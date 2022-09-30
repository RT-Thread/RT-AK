/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Sensor abstraction layer.
 */
#include <stdlib.h>
#include <string.h>
#include "mp.h"
#define __log(_var) mp_printf(&mp_plat_print,"%s,%d," #_var "=0x%x\n",__FILE__,__LINE__,(_var));
#include "cambus.h"
#include "sensor.h"
#include "systick.h"
#include "framebuffer.h"
#include "omv_boardconfig.h"

#if (OMV_ENABLE_OV2640 == 1)
#include "cameras/ov2640.h"
#endif

#if (OMV_ENABLE_OV5640 == 1)
#include "cameras/ov5640.h"
#endif

#if (OMV_ENABLE_GC0328 == 1)
#include "cameras/gc0328.h"
#endif

#define MAX_XFER_SIZE   (0xFFFF*4)

extern void __fatal_error(const char *msg);

sensor_t           sensor     = {0};

extern uint8_t _line_buf;
static uint8_t *dest_fb = NULL;
static volatile int offset = 0;
static volatile bool jpeg_buffer_overflow = false;
static volatile bool waiting_for_data = false;

#define DCMI_PWDN_HIGH() cambus_dcmi_pwdn(&sensor.cambus, 1)
#define DCMI_PWDN_LOW()  cambus_dcmi_pwdn(&sensor.cambus, 0)
#define DCMI_RESET_HIGH()  cambus_dcmi_reset(&sensor.cambus, 1)
#define DCMI_RESET_LOW()   cambus_dcmi_reset(&sensor.cambus, 0)

const int resolution[][2] = {
    {0,    0   },
    // C/SIF Resolutions
    {88,   72  },    /* QQCIF     */
    {176,  144 },    /* QCIF      */
    {352,  288 },    /* CIF       */
    {88,   60  },    /* QQSIF     */
    {176,  120 },    /* QSIF      */
    {352,  240 },    /* SIF       */
    // VGA Resolutions
    {40,   30  },    /* QQQQVGA   */
    {80,   60  },    /* QQQVGA    */
    {160,  120 },    /* QQVGA     */
    {320,  240 },    /* QVGA      */
    {640,  480 },    /* VGA       */
    {60,   40  },    /* HQQQVGA   */
    {120,  80  },    /* HQQVGA    */
    {240,  160 },    /* HQVGA     */
    // FFT Resolutions
    {64,   32  },    /* 64x32     */
    {64,   64  },    /* 64x64     */
    {128,  64  },    /* 128x64    */
    {128,  128 },    /* 128x128   */
    // Other
    {128,  160 },    /* LCD       */
    {128,  160 },    /* QQVGA2    */
    {720,  480 },    /* WVGA      */
    {752,  480 },    /* WVGA2     */
    {800,  600 },    /* SVGA      */
    {1024, 768 },    /* XGA       */
    {1280, 1024},    /* SXGA      */
    {1600, 1200},    /* UXGA      */
    {1280, 720 },    /* HD        */
    {1920, 1080},    /* FHD       */
    {2560, 1440},    /* QHD       */
    {2048, 1536},    /* QXGA      */
    {2560, 1600},    /* WQXGA     */
    {2592, 1944},    /* WQXGA2    */
};

static int dcmi_config(uint32_t mode)
{
    cambus_set_pixformat(&sensor.cambus, &mode);

    return 0;
}

static void dcmi_abort(void)
{
    // This stops the DCMI hardware from generating DMA requests immediately and then stops the DMA
    // hardware. Note that HAL_DMA_Abort is a blocking operation. Do not use this in an interrupt.
    
    cambus_dcmi_abort(&sensor.cambus);
}

static int extclk_config(uint32_t freq)
{
    cambus_set_freq(&sensor.cambus, 0, freq);

    return 0;
}

// Returns true if a crop is being applied to the frame buffer.
static bool cropped()
{
    return MAIN_FB()->x // needs to be zero if not being cropped.
        || MAIN_FB()->y // needs to be zero if not being cropped.
        || (MAIN_FB()->u != resolution[sensor.framesize][0])  // should be equal to the resolution if not cropped.
        || (MAIN_FB()->v != resolution[sensor.framesize][1]); // should be equal to the resolution if not cropped.
}

void sensor_init0()
{
    dcmi_abort();

    // Save fb_enabled flag state
    int fb_enabled = JPEG_FB()->enabled;

    // Clear framebuffers
    memset(MAIN_FB(), 0, sizeof(*MAIN_FB()));
    memset(JPEG_FB(), 0, sizeof(*JPEG_FB()));

    // Skip the first frame.
    MAIN_FB()->bpp = -1;

    // Enable streaming.
    MAIN_FB()->streaming_enabled = true; // controlled by the OpenMV Cam.

    // Set default quality
    JPEG_FB()->quality = ((JPEG_QUALITY_HIGH - JPEG_QUALITY_LOW) / 2) + JPEG_QUALITY_LOW;

    // Set fb_enabled
    JPEG_FB()->enabled = fb_enabled; // controlled by the IDE.
    #ifdef PORTENTA
    // The Portenta board uses the same I2C bus for the sensor and
    // user scripts. The I2C bus must be reinitialized on soft-reset.
    cambus_init(&sensor.cambus, SCCB_TIMING);
    #endif

}

int sensor_init()
{
    int init_ret = 0;

    /* Reset the sesnor state */
    memset(&sensor, 0, sizeof(sensor_t));

     /* GC0308 detect */
    cambus_init_gc0308(&sensor.cambus);
     /* Do a power cycle */
    DCMI_PWDN_HIGH();
    systick_sleep(10);

    DCMI_PWDN_LOW();
    systick_sleep(10);

    sensor.chip_id = cambus_scan_gc0308();
    if(sensor.chip_id){
        sensor.slv_addr = GC0308_ADDR;
        sensor.snapshot = sensor_snapshot;
        goto _gc0308;
    }else
    {
        cambus_deinit_gc0308();
    }

    /* GC0328 detect */
    cambus_init_gc0328(&sensor.cambus);
     /* Do a power cycle */
    DCMI_PWDN_HIGH();
    systick_sleep(10);

    DCMI_PWDN_LOW();
    systick_sleep(10);

    sensor.chip_id = cambus_scan_gc0328();
    if(sensor.chip_id){
        sensor.slv_addr = GC0328_ADDR;
        sensor.snapshot = sensor_snapshot;
        goto _gc0328;
    }else
    {
        cambus_deinit_gc0328();
    }

    // Initialize the camera bus.
    cambus_init(&sensor.cambus);

    /* Do a power cycle */
    DCMI_PWDN_HIGH();
    systick_sleep(10);

    DCMI_PWDN_LOW();
    systick_sleep(10);

    /* Some sensors have different reset polarities, and we can't know which sensor
       is connected before initializing cambus and probing the sensor, which in turn
       requires pulling the sensor out of the reset state. So we try to probe the
       sensor with both polarities to determine line state. */
    sensor.pwdn_pol = ACTIVE_HIGH;
    sensor.reset_pol = ACTIVE_HIGH;

    /* Reset the sensor */
    DCMI_RESET_HIGH();
    systick_sleep(10);

    DCMI_RESET_LOW();
    systick_sleep(10);

    systick_sleep(10);

    /* Probe the sensor */
    sensor.slv_addr = cambus_scan(&sensor.cambus);__log(0)
    if (sensor.slv_addr == 0) {
        /* Sensor has been held in reset,
           so the reset line is active low */
        sensor.reset_pol = ACTIVE_LOW;

        /* Pull the sensor out of the reset state */
        DCMI_RESET_HIGH();
        systick_sleep(10);

        /* Probe again to set the slave addr */
        sensor.slv_addr = cambus_scan(&sensor.cambus);
        if (sensor.slv_addr == 0) {
            sensor.pwdn_pol = ACTIVE_LOW;

            DCMI_PWDN_HIGH();
            systick_sleep(10);

            sensor.slv_addr = cambus_scan(&sensor.cambus);
            if (sensor.slv_addr == 0) {
                sensor.reset_pol = ACTIVE_HIGH;

                DCMI_RESET_LOW();
                systick_sleep(10);

                sensor.slv_addr = cambus_scan(&sensor.cambus);
                if (sensor.slv_addr == 0) {__log(0)
                    return -2;
                }
            }
        }
    }

    // Clear sensor chip ID.
    sensor.chip_id = 0;

    // Set default snapshot function.
    sensor.snapshot = sensor_snapshot;

    switch (sensor.slv_addr) {
        case GC0308_ADDR: // Or OV9650.
            cambus_readb(&sensor.cambus, sensor.slv_addr, GC0308_CHIP_ID, &sensor.chip_id);
            __log(sensor.chip_id)
            break;
        case OV2640_SLV_ADDR: // Or OV9650.
            cambus_readb(&sensor.cambus, sensor.slv_addr, OV_CHIP_ID, &sensor.chip_id);
            break;
        case OV5640_SLV_ADDR:
            cambus_readb2(&sensor.cambus, sensor.slv_addr, OV5640_CHIP_ID, &sensor.chip_id);
            break;
        // case OV7725_SLV_ADDR: // Or OV7690. conficated with GC0308
        //     cambus_readb(&sensor.cambus, sensor.slv_addr, OV_CHIP_ID, &sensor.chip_id);
        //     break;
        case MT9V034_SLV_ADDR:
            cambus_readb(&sensor.cambus, sensor.slv_addr, ON_CHIP_ID, &sensor.chip_id);
            break;
        case LEPTON_SLV_ADDR:
            sensor.chip_id = LEPTON_ID;
            break;
        case HM01B0_SLV_ADDR:
            cambus_readb2(&sensor.cambus, sensor.slv_addr, HIMAX_CHIP_ID, &sensor.chip_id);
            break;
        default:
            return -3;
            break;
    }
    _gc0308:
    _gc0328:
    switch (sensor.chip_id) {
        #if (OMV_ENABLE_GC0308 == 1)
        case GC0308_ID:
            init_ret = gc0308_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV2640 == 1)
        #if (OMV_ENABLE_GC0328 == 1)
        case GC0328_ID:
            init_ret = gc0328_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV2640 == 1)
        #if (OMV_ENABLE_OV2640 == 1)
        case OV2640_ID:
            if (extclk_config(OV2640_XCLK_FREQ) != 0) {
                return -3;
            }
            init_ret = ov2640_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV2640 == 1)

        #if (OMV_ENABLE_OV5640 == 1)
        case OV5640_ID:
            if (extclk_config(OV5640_XCLK_FREQ) != 0) {
                return -3;
            }
            init_ret = ov5640_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV5640 == 1)

        #if (OMV_ENABLE_OV7690 == 1)
        case OV7690_ID:
            if (extclk_config(OV7690_XCLK_FREQ) != 0) {
                return -3;
            }
            init_ret = ov7690_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV7690 == 1)

        #if (OMV_ENABLE_OV7725 == 1)
        case OV7725_ID:
            init_ret = ov7725_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV7725 == 1)

        #if (OMV_ENABLE_OV9650 == 1)
        case OV9650_ID:
            init_ret = ov9650_init(&sensor);
            break;
        #endif // (OMV_ENABLE_OV9650 == 1)

        #if (OMV_ENABLE_MT9V034 == 1)
        case MT9V034_ID:
            if (extclk_config(MT9V034_XCLK_FREQ) != 0) {
                return -3;
            }
            init_ret = mt9v034_init(&sensor);
            break;
        #endif //(OMV_ENABLE_MT9V034 == 1)

        #if (OMV_ENABLE_LEPTON == 1)
        case LEPTON_ID:
            if (extclk_config(LEPTON_XCLK_FREQ) != 0) {
                return -3;
            }
            init_ret = lepton_init(&sensor);
            break;
        #endif // (OMV_ENABLE_LEPTON == 1)

        #if (OMV_ENABLE_HM01B0 == 1)
        case HM01B0_ID:
            init_ret = hm01b0_init(&sensor);
            break;
        #endif //(OMV_ENABLE_HM01B0 == 1)

        default:
            return -3;
            break;
    }

    if (init_ret != 0 ) {__log(0)
        // Sensor init failed.
        return -4;
    }

    // Set default color palette.
    //TODO sensor.color_palette = rainbow_table;

    // Enable streaming.
    MAIN_FB()->streaming_enabled = true; // controlled by the OpenMV Cam.

    // Set default quality
    JPEG_FB()->quality = ((JPEG_QUALITY_HIGH - JPEG_QUALITY_LOW) / 2) + JPEG_QUALITY_LOW;

    /* All good! */
    sensor.initflag = 0x107D0E;

    return 0;
}

int sensor_reset()
{
    if (sensor_init() != 0)
        return -1;

    dcmi_abort();

    // Reset the sensor state
    sensor.sde           = 0;
    sensor.pixformat     = 0;
    sensor.framesize     = 0;
    sensor.gainceiling   = 0;
    sensor.hmirror       = false;
    sensor.vflip         = false;
    sensor.transpose     = false;
    #if MICROPY_PY_IMU
    sensor.auto_rotation = sensor.chip_id == OV7690_ID;
    #else
    sensor.auto_rotation = false;
    #endif // MICROPY_PY_IMU
    sensor.vsync_gpio    = NULL;

    // Reset default color palette.
    //TODO sensor.color_palette = rainbow_table;

    // Restore shutdown state on reset.
    sensor_shutdown(false);

    // Call sensor-specific reset function
    if (sensor.reset(&sensor) != 0) {
        return -1;
    }

    return 0;
}

int sensor_get_id()
{
    return sensor.chip_id;
}

bool sensor_is_detected()
{
    return (sensor.initflag == 0x107D0E);
}

int sensor_sleep(int enable)
{
    dcmi_abort();

    if (sensor.sleep == NULL
        || sensor.sleep(&sensor, enable) != 0) {
        // Operation not supported
        return -1;
    }
    return 0;
}

int sensor_shutdown(int enable)
{
    dcmi_abort();

    if (enable) {
        DCMI_PWDN_HIGH();
    } else {
        DCMI_PWDN_LOW();
    }

    systick_sleep(10);
    return 0;
}

int sensor_read_reg(uint16_t reg_addr)
{
    if (sensor.read_reg == NULL) {
        // Operation not supported
        return -1;
    }
    return sensor.read_reg(&sensor, reg_addr);
}

int sensor_write_reg(uint16_t reg_addr, uint16_t reg_data)
{
    if (sensor.write_reg == NULL) {
        // Operation not supported
        return -1;
    }
    return sensor.write_reg(&sensor, reg_addr, reg_data);
}

int sensor_set_pixformat(pixformat_t pixformat)
{
    pixformat_t spf = pixformat;

    if (sensor.pixformat == pixformat) {
        // No change
        return 0;
    }

    // sensor_check_buffsize() will switch from PIXFORMAT_BAYER to PIXFORMAT_RGB565 to try to fit
    // the MAIN_FB() in RAM as a first step optimization. If the user tries to switch back to RGB565
    // and that would be bigger than the RAM buffer we would just switch back.
    //
    // So, just short-circuit doing any work.
    //
    // This code is explicitly here to allow users to set the resolution to RGB565 and have it
    // switch to BAYER only once even though they are setting the resolution to RGB565 repeatedly
    // in a loop. Only RGB565->BAYER has this problem and needs this fix because of sensor_check_buffsize().
    uint32_t size = framebuffer_get_buffer_size();
    if ((sensor.pixformat == PIXFORMAT_BAYER)
    &&  (pixformat == PIXFORMAT_RGB565)
    &&  (MAIN_FB()->u * MAIN_FB()->v * 2 > size)
    &&  (MAIN_FB()->u * MAIN_FB()->v * 1 <= size)) {
        // No change
        return 0;
    }

    // Cropping and transposing (and thus auto rotation) don't work in JPEG mode.
    if ((pixformat == PIXFORMAT_JPEG) && (cropped() || sensor.transpose || sensor.auto_rotation)) {
        return -1;
    }

    dcmi_abort();

    cambus_sensor_pixformat_modify(&sensor.cambus, (int*)&spf);
    if (sensor.set_pixformat == NULL
        || sensor.set_pixformat(&sensor, spf) != 0) {
        // Operation not supported
        return -1;
    }

    systick_sleep(100); // wait for the camera to settle

    if (cambus_set_pixformat(&sensor.cambus, (int*)&pixformat) != 0)
        return -1;

    // Set pixel format
    sensor.pixformat = pixformat;

    // Skip the first frame.
    MAIN_FB()->bpp = -1;

    // Change the JPEG mode.
    return 0;
}

int sensor_set_framesize(framesize_t framesize)
{
    int w_old = MAIN_FB()->w;
    int h_old = MAIN_FB()->h;

    if (sensor.framesize == framesize) {
        // No change
        return 0;
    }

    dcmi_abort();

    // Call the sensor specific function
    if (sensor.set_framesize == NULL
        || sensor.set_framesize(&sensor, framesize) != 0) {
        // Operation not supported
        return -1;
    }

    cambus_set_image_size(&sensor.cambus, resolution[framesize][0], resolution[framesize][1]);

    systick_sleep(100); // wait for the camera to settle

    // Set framebuffer size
    sensor.framesize = framesize;

    // Skip the first frame.
    MAIN_FB()->bpp = -1;

    // Set MAIN FB x offset, y offset, width, height, backup width, and backup height.
    MAIN_FB()->x = 0;
    MAIN_FB()->y = 0;
    MAIN_FB()->w = MAIN_FB()->u = resolution[framesize][0];
    MAIN_FB()->h = MAIN_FB()->v = resolution[framesize][1];

    return 0;
}

int sensor_set_windowing(int x, int y, int w, int h)
{
    // py_sensor_set_windowing ensures this the window is at least 8x8
    // and that it is fully inside the sensor output framesize window.
    if (sensor.pixformat == PIXFORMAT_JPEG) {
        return -1;
    }

    // We force everything to be a multiple of 2 so that when you switch between
    // grayscale/rgb565/bayer/jpeg the frame doesn't need to move around for bayer to work.
    MAIN_FB()->x = (x / 2) * 2;
    MAIN_FB()->y = (y / 2) * 2;
    MAIN_FB()->w = MAIN_FB()->u = (w / 2) * 2;
    MAIN_FB()->h = MAIN_FB()->v = (h / 2) * 2;

    return 0;
}

int sensor_set_contrast(int level)
{
    if (sensor.set_contrast != NULL) {
        return sensor.set_contrast(&sensor, level);
    }
    return -1;
}

int sensor_set_brightness(int level)
{
    if (sensor.set_brightness != NULL) {
        return sensor.set_brightness(&sensor, level);
    }
    return -1;
}

int sensor_set_saturation(int level)
{
    if (sensor.set_saturation != NULL) {
        return sensor.set_saturation(&sensor, level);
    }
    return -1;
}

int sensor_set_gainceiling(gainceiling_t gainceiling)
{
    if (sensor.gainceiling == gainceiling) {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    if (sensor.set_gainceiling == NULL
        || sensor.set_gainceiling(&sensor, gainceiling) != 0) {
        /* operation not supported */
        return -1;
    }

    sensor.gainceiling = gainceiling;
    return 0;
}

int sensor_set_quality(int qs)
{
    /* call the sensor specific function */
    if (sensor.set_quality == NULL
        || sensor.set_quality(&sensor, qs) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_colorbar(int enable)
{
    /* call the sensor specific function */
    if (sensor.set_colorbar == NULL
        || sensor.set_colorbar(&sensor, enable) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_auto_gain(int enable, float gain_db, float gain_db_ceiling)
{
    /* call the sensor specific function */
    if (sensor.set_auto_gain == NULL
        || sensor.set_auto_gain(&sensor, enable, gain_db, gain_db_ceiling) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_get_gain_db(float *gain_db)
{
    /* call the sensor specific function */
    if (sensor.get_gain_db == NULL
        || sensor.get_gain_db(&sensor, gain_db) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_auto_exposure(int enable, int exposure_us)
{
    /* call the sensor specific function */
    if (sensor.set_auto_exposure == NULL
        || sensor.set_auto_exposure(&sensor, enable, exposure_us) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_get_exposure_us(int *exposure_us)
{
    /* call the sensor specific function */
    if (sensor.get_exposure_us == NULL
        || sensor.get_exposure_us(&sensor, exposure_us) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_auto_whitebal(int enable, float r_gain_db, float g_gain_db, float b_gain_db)
{
    /* call the sensor specific function */
    if (sensor.set_auto_whitebal == NULL
        || sensor.set_auto_whitebal(&sensor, enable, r_gain_db, g_gain_db, b_gain_db) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_get_rgb_gain_db(float *r_gain_db, float *g_gain_db, float *b_gain_db)
{
    /* call the sensor specific function */
    if (sensor.get_rgb_gain_db == NULL
        || sensor.get_rgb_gain_db(&sensor, r_gain_db, g_gain_db, b_gain_db) != 0) {
        /* operation not supported */
        return -1;
    }
    return 0;
}

int sensor_set_hmirror(int enable)
{
    if (sensor.hmirror == ((bool) enable)) {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    if (sensor.set_hmirror == NULL
        || sensor.set_hmirror(&sensor, enable) != 0) {
        /* operation not supported */
        return -1;
    }
    sensor.hmirror = enable;
    systick_sleep(100); // wait for the camera to settle
    return 0;
}

bool sensor_get_hmirror()
{
    return sensor.hmirror;
}

int sensor_set_vflip(int enable)
{
    if (sensor.vflip == ((bool) enable)) {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    if (sensor.set_vflip == NULL
        || sensor.set_vflip(&sensor, enable) != 0) {
        /* operation not supported */
        return -1;
    }
    sensor.vflip = enable;
    systick_sleep(100); // wait for the camera to settle
    return 0;
}

bool sensor_get_vflip()
{
    return sensor.vflip;
}

int sensor_set_transpose(bool enable)
{
    if (sensor.pixformat == PIXFORMAT_JPEG) {
        return -1;
    }

    sensor.transpose = enable;
    return 0;
}

bool sensor_get_transpose()
{
    return sensor.transpose;
}

int sensor_set_auto_rotation(bool enable)
{
    if (sensor.pixformat == PIXFORMAT_JPEG) {
        return -1;
    }

    sensor.auto_rotation = enable;
    return 0;
}

bool sensor_get_auto_rotation()
{
    return sensor.auto_rotation;
}

int sensor_set_special_effect(sde_t sde)
{
    if (sensor.sde == sde) {
        /* no change */
        return 0;
    }

    /* call the sensor specific function */
    if (sensor.set_special_effect == NULL
        || sensor.set_special_effect(&sensor, sde) != 0) {
        /* operation not supported */
        return -1;
    }

    sensor.sde = sde;
    return 0;
}

int sensor_set_lens_correction(int enable, int radi, int coef)
{
    /* call the sensor specific function */
    if (sensor.set_lens_correction == NULL
        || sensor.set_lens_correction(&sensor, enable, radi, coef) != 0) {
        /* operation not supported */
        return -1;
    }

    return 0;
}

int sensor_ioctl(int request, ... /* arg */)
{
    dcmi_abort();

    int ret = -1;

    if (sensor.ioctl != NULL) {
        va_list ap;
        va_start(ap, request);
        /* call the sensor specific function */
        ret = sensor.ioctl(&sensor, request, ap);
        va_end(ap);
    }

    return ret;
}

int sensor_set_vsync_output(void *gpio, uint32_t pin)
{
    sensor.vsync_pin  = pin;
    sensor.vsync_gpio = gpio;
    // Enable VSYNC EXTI IRQ
#if 0//TODO
    NVIC_SetPriority(DCMI_VSYNC_IRQN, IRQ_PRI_EXTINT);
    HAL_NVIC_EnableIRQ(DCMI_VSYNC_IRQN);
#endif
    return 0;
}

int sensor_set_color_palette(const uint16_t *color_palette)
{
    sensor.color_palette = color_palette;
    return 0;
}

const uint16_t *sensor_get_color_palette()
{
    return sensor.color_palette;
}

// To make the user experience better we automatically shrink the size of the MAIN_FB() to fit
// within the RAM we have onboard the system.
static void sensor_check_buffsize()
{
    uint32_t size = framebuffer_get_buffer_size();
    uint32_t bpp;

    switch (sensor.pixformat) {
        case PIXFORMAT_GRAYSCALE:
        case PIXFORMAT_BAYER:
            bpp = 1;
            break;
        case PIXFORMAT_RGB565:
        case PIXFORMAT_YUV422:
            bpp = 2;
            break;
        // If the pixformat is NULL/JPEG there we can't do anything to check if it fits before hand.
        default:
            return;
    }

    // MAIN_FB() fits, we are done.
    if ((MAIN_FB()->u * MAIN_FB()->v * bpp) <= size) {
        return;
    }

    if (sensor.pixformat == PIXFORMAT_RGB565) {
        // Switch to bayer for the quick 2x savings.
        sensor_set_pixformat(PIXFORMAT_BAYER);
        bpp = 1;

        // MAIN_FB() fits, we are done (bpp is 1).
        if (MAIN_FB()->u * MAIN_FB()->v <= size) {
            return;
        }
    }

    int window_w = MAIN_FB()->u;
    int window_h = MAIN_FB()->v;

    // We need to shrink the frame buffer. We can do this by cropping. So, we will subtract columns
    // and rows from the frame buffer until it fits within the frame buffer.
    int max = IM_MAX(window_w, window_h);
    int min = IM_MIN(window_w, window_h);
    float aspect_ratio = max / ((float) min);
    float r = aspect_ratio, best_r = r;
    int c = 1, best_c = c;
    float best_err = FLT_MAX;

    // Find the width/height ratio that's within 1% of the aspect ratio with a loop limit.
    for (int i = 100; i; i--) {
        float err = fabsf(r - roundf(r));

        if (err <= best_err) {
            best_err = err;
            best_r = r;
            best_c = c;
        }

        if (best_err <= 0.01f) {
            break;
        }

        r += aspect_ratio;
        c += 1;
    }

    // Select the larger geometry to map the aspect ratio to.
    int u_sub, v_sub;

    if (window_w > window_h) {
        u_sub = roundf(best_r);
        v_sub = best_c;
    } else {
        u_sub = best_c;
        v_sub = roundf(best_r);
    }

    // Crop the frame buffer while keeping the aspect ratio and keeping the width/height even.
    while (((MAIN_FB()->u * MAIN_FB()->v * bpp) > size) || (MAIN_FB()->u % 2)  || (MAIN_FB()->v % 2)) {
        MAIN_FB()->u -= u_sub;
        MAIN_FB()->v -= v_sub;
    }

    // Center the new window using the previous offset and keep the offset even.
    MAIN_FB()->x += (window_w - MAIN_FB()->u) / 2;
    MAIN_FB()->y += (window_h - MAIN_FB()->v) / 2;
    if (MAIN_FB()->x % 2) MAIN_FB()->x -= 1;
    if (MAIN_FB()->y % 2) MAIN_FB()->y -= 1;
}

// ARM Cortex-M4/M7 Processors can access memory using unaligned 32-bit reads/writes.
void *unaligned_2_to_1_memcpy(void *dest, void *src, size_t n)
{
    uint32_t *dest32 = (uint32_t *) dest;
    uint32_t *src32 = (uint32_t *) src;

// TODO: Make this faster using only 32-bit aligned reads/writes with data shifting.
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
    for (; n > 4; n -= 4) {
        uint32_t tmp1 = *src32++;
        uint32_t tmp2 = *src32++;
        *dest32++ = (tmp1 & 0xff) | ((tmp1 >> 8) & 0xff00) | ((tmp2 & 0xff) << 16) | ((tmp2 & 0xff0000) << 8);
    }
#endif

    uint8_t *dest8 = (uint8_t *) dest32;
    uint16_t *src16 = (uint16_t *) src32;

    for (; n > 0; n -= 1) {
        *dest8++ = *src16++;
    }

    return dest;
}

// ARM Cortex-M4/M7 Processors can access memory using unaligned 32-bit reads/writes.
void *unaligned_memcpy(void *dest, void *src, size_t n)
{
// TODO: Make this faster using only 32-bit aligned reads/writes with data shifting.
#if defined(MCU_SERIES_F4) || defined(MCU_SERIES_F7) || defined(MCU_SERIES_H7)
    uint32_t *dest32 = (uint32_t *) dest;
    uint32_t *src32 = (uint32_t *) src;

    for (; n > 4; n -= 4) {
        *dest32++ = *src32++;
    }

    uint8_t *dest8 = (uint8_t *) dest32;
    uint8_t *src8 = (uint8_t *) src32;

    for (; n > 0; n -= 1) {
        *dest8++ = *src8++;
    }

    return dest;
#else
    return memcpy(dest, src, n);
#endif
}
#if 0//TODO
// Stop allowing new data in on the end of the frame and let snapshot know that the frame has been
// received. Note that DCMI_DMAConvCpltUser() is called before DCMI_IT_FRAME is enabled by
// DCMI_DMAXferCplt() so this means that the last line of data is *always* transferred before
// waiting_for_data is set to false.
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
    waiting_for_data = false;
}
#endif

// This is the default snapshot function, which can be replaced in sensor_init functions. This function
// uses the DCMI and DMA to capture frames and each line is processed in the DCMI_DMAConvCpltUser function.
int sensor_snapshot(sensor_t *sensor, image_t *image, streaming_cb_t streaming_cb)
{
    bool streaming = (streaming_cb != NULL); // Streaming mode.

    // In streaming mode the image pointer must be valid.
    if (streaming) {
        if (image == NULL) {
            return -1;
        }

        // Clear the first image in to not trigger the streaming_cb in double buffer mode.
        image->pixels = NULL;
    }

    // Compress the framebuffer for the IDE preview, only if it's not the first frame,
    // the framebuffer is enabled and the image sensor does not support JPEG encoding.
    // Note: This doesn't run unless the IDE is connected and the framebuffer is enabled.
    fb_update_jpeg_buffer();
    
    // Make sure the raw frame fits into the FB. It will be switched from RGB565 to BAYER
    // first to save space before being cropped until it fits.
    sensor_check_buffsize();

    if (cambus_read_pixels(&sensor->cambus, MAIN_FB()->pixels, 200) != 0)
        return -5;

    // Fix the BPP.
    switch (sensor->pixformat)
    {
    case PIXFORMAT_GRAYSCALE:
        MAIN_FB()->bpp = 1;
        framebuffer_set(sensor->cambus.width, sensor->cambus.height, IMAGE_BPP_GRAYSCALE);
        break;
    case PIXFORMAT_YUV422:
    case PIXFORMAT_RGB565:
        MAIN_FB()->bpp = 2;
        framebuffer_set(sensor->cambus.width, sensor->cambus.height, IMAGE_BPP_RGB565);
        break;
    case PIXFORMAT_BAYER:
        MAIN_FB()->bpp = 3;
        framebuffer_set(sensor->cambus.width, sensor->cambus.height, IMAGE_BPP_BAYER);
        break;
    }

    image->w = MAIN_FB()->w;
    image->h = MAIN_FB()->h;
    image->bpp = MAIN_FB()->bpp;
    image->pixels = MAIN_FB()->pixels;
    image->ai_pixels = MAIN_FB()->pixels + OMV_RAW_BUF_SIZE;

    return 0;
}

void sensor_switch(int sel){
    if((sensor.chip_id != GC0328_ID) || (sensor.slv_addr != GC0328_ADDR)){
        return;
    }
    if(sel == 0){
        open_gc0328_0();
    }
    if(sel == 1){
        open_gc0328_1();
    }
    systick_sleep(1);
}