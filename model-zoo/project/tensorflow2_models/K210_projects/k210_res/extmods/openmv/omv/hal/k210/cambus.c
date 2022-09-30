/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * SCCB (I2C like) driver.
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "systick.h"
#include "omv_boardconfig.h"
#include "cambus.h"
#include "sensor.h"
#include "plic.h"
#include "dvp.h"
#include "fpioa.h"
#include "sysctl.h"
#include "i2c.h"
#include "sleep.h"
#include <rtthread.h>

static int g_dvp_finish_flag = 0;

#define I2C_FREQUENCY (100000)
#define I2C_TIMEOUT (1000)

// GC0328
#define GC0328_ID       (0x9d)
#define GC0328_ADDR     (0x42 >> 1)
void sccb_i2c_switch(int num)
{
	if (num)
	{
		fpioa_set_function(BSP_CAMERA_SCCB_SDA0_PIN, FUNC_RESV0);
		fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN, FUNC_I2C0_SDA);
		fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_I2C0_SCLK);
	}
	else
	{
		fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN, FUNC_RESV0);
		fpioa_set_function(BSP_CAMERA_SCCB_SDA0_PIN, FUNC_I2C0_SDA);
		fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_I2C0_SCLK);
	}
	i2c_init(0, GC0328_ADDR , 7, 100000);
}

static void gc0328_wr_reg(uint8_t num, uint8_t reg,uint8_t data)
{
    uint8_t buf[2];
    buf[0] = reg & 0xff;
    buf[1] = data;
    i2c_send_data(num, buf, 2);
}

static uint8_t gc0328_rd_reg(uint8_t num, uint8_t reg)
{
    uint8_t reg_buf[1];
    uint8_t data_buf;

    reg_buf[0] = reg & 0xff; 
    i2c_recv_data(num, reg_buf, 1, &data_buf, 1);
    return data_buf;
}


static int sccb_i2c_read_byte0(int8_t i2c, uint8_t addr, uint16_t reg, uint8_t reg_len, uint8_t *data, uint16_t timeout_ms)
{
    *data = 0;
    // *data = dvp_sccb_receive_data(addr, reg);
    *data = gc0328_rd_reg(i2c,  reg);

    return 0;
}

static int sccb_i2c_write_byte0(int8_t i2c, uint8_t addr, uint16_t reg, uint8_t reg_len, uint8_t data, uint16_t timeout_ms)
{
    // dvp_sccb_send_data(addr, reg, data);
    gc0328_wr_reg(i2c, reg, data);

    return 0;
}

int cambus_readb3( uint8_t slv_addr, uint16_t reg_addr, uint8_t *reg_data)
{
    int ret = 0;

    sccb_i2c_read_byte0(0, slv_addr, reg_addr, 0, reg_data, 10);
    if (0xff == *reg_data)
        ret = -1;

    return ret;
}

int cambus_writeb3(uint8_t slv_addr, uint16_t reg_addr, uint8_t reg_data)
{
    int ret = 0;

    sccb_i2c_write_byte0(0, slv_addr, reg_addr, 0, reg_data, 10);

    return ret;
}

void open_gc0328_0()
{
    usleep(1 * 1000);
	sccb_i2c_switch(1);
    usleep(1 * 1000);
    cambus_writeb3(GC0328_ADDR, 0xF1, 0x00);
    cambus_writeb3(GC0328_ADDR, 0xFE, 0x00);
    cambus_writeb3(GC0328_ADDR, 0xF2, 0x00);

    usleep(1 * 1000);
	sccb_i2c_switch(0);
    usleep(1 * 1000);
    cambus_writeb3(GC0328_ADDR, 0xFE, 0x00);
    cambus_writeb3(GC0328_ADDR, 0xF1, 0x07);
    cambus_writeb3(GC0328_ADDR, 0xF2, 0x01);
}

void open_gc0328_1()
{
	sccb_i2c_switch(0);
	cambus_writeb3(GC0328_ADDR, 0xFE, 0x00);
	cambus_writeb3(GC0328_ADDR, 0xF1, 0x00);
	cambus_writeb3(GC0328_ADDR, 0xF2, 0x00);

	usleep(1 * 1000);
	sccb_i2c_switch(1);
	cambus_writeb3(GC0328_ADDR, 0xFE, 0x00);
	cambus_writeb3(GC0328_ADDR, 0xF1, 0x07);
	cambus_writeb3(GC0328_ADDR, 0xF2, 0x01);
}
// GC0328
// GC0308   
static void gc0308_wr_reg(uint8_t num, uint8_t reg, uint8_t data)
{
    uint8_t buf[2];

    buf[0] = reg & 0xff;
    buf[1] = data;
	clear_csr(mie, MIP_MTIP);
    i2c_send_data(num, buf, 2);
	set_csr(mie, MIP_MTIP);
}

static uint8_t gc0308_rd_reg(uint8_t num, uint8_t reg)
{
    uint8_t reg_buf[1];
    uint8_t data_buf;

    reg_buf[0] = reg & 0xff;
	clear_csr(mie, MIP_MTIP);
    i2c_recv_data(num, reg_buf, 1, &data_buf, 1);
	set_csr(mie, MIP_MTIP);
    return data_buf;
}

static void dvp_sccb_start_transfer(void)
{
    while (dvp->sts & DVP_STS_SCCB_EN)
        ;
    dvp->sts = DVP_STS_SCCB_EN | DVP_STS_SCCB_EN_WE;
    while (dvp->sts & DVP_STS_SCCB_EN)
        ;
}

static void _dvp_sccb_send_data(uint8_t dev_addr, uint16_t reg_addr, uint8_t reg_data, int g_sccb_reg_len)
{
    uint32_t tmp;

    tmp = dvp->sccb_cfg & (~DVP_SCCB_BYTE_NUM_MASK);

    (g_sccb_reg_len == 8) ? (tmp |= DVP_SCCB_BYTE_NUM_3) : (tmp |= DVP_SCCB_BYTE_NUM_4);

    dvp->sccb_cfg = tmp;

    if (g_sccb_reg_len == 8)
    {
        dvp->sccb_ctl = DVP_SCCB_WRITE_DATA_ENABLE | DVP_SCCB_DEVICE_ADDRESS(dev_addr) | DVP_SCCB_REG_ADDRESS(reg_addr) | DVP_SCCB_WDATA_BYTE0(reg_data);
    }
    else
    {
        dvp->sccb_ctl = DVP_SCCB_WRITE_DATA_ENABLE | DVP_SCCB_DEVICE_ADDRESS(dev_addr) | DVP_SCCB_REG_ADDRESS(reg_addr >> 8) | DVP_SCCB_WDATA_BYTE0(reg_addr & 0xff) | DVP_SCCB_WDATA_BYTE1(reg_data);
    }
    dvp_sccb_start_transfer();
}

static uint8_t _dvp_sccb_receive_data(uint8_t dev_addr, uint16_t reg_addr, int g_sccb_reg_len)
{
    uint32_t tmp;

    tmp = dvp->sccb_cfg & (~DVP_SCCB_BYTE_NUM_MASK);

    if (g_sccb_reg_len == 8)
        tmp |= DVP_SCCB_BYTE_NUM_2;
    else
        tmp |= DVP_SCCB_BYTE_NUM_3;

    dvp->sccb_cfg = tmp;

    if (g_sccb_reg_len == 8)
    {
        dvp->sccb_ctl = DVP_SCCB_WRITE_DATA_ENABLE | DVP_SCCB_DEVICE_ADDRESS(dev_addr) | DVP_SCCB_REG_ADDRESS(reg_addr);
    }
    else
    {
        dvp->sccb_ctl = DVP_SCCB_WRITE_DATA_ENABLE | DVP_SCCB_DEVICE_ADDRESS(dev_addr) | DVP_SCCB_REG_ADDRESS(reg_addr >> 8) | DVP_SCCB_WDATA_BYTE0(reg_addr & 0xff);
    }
    dvp_sccb_start_transfer();

    dvp->sccb_ctl = DVP_SCCB_DEVICE_ADDRESS(dev_addr);

    dvp_sccb_start_transfer();

    return (uint8_t) DVP_SCCB_RDATA_BYTE(dvp->sccb_cfg);
}

static int sccb_i2c_read_byte(int8_t i2c, uint8_t addr, uint16_t reg, uint8_t reg_len, uint8_t *data, uint16_t timeout_ms)
{
    *data = 0;

    *data = _dvp_sccb_receive_data(addr, reg, reg_len);

    return 0;
}

static int sccb_i2c_write_byte(int8_t i2c, uint8_t addr, uint16_t reg, uint8_t reg_len, uint8_t data, uint16_t timeout_ms)
{
    _dvp_sccb_send_data(addr, reg, data, reg_len);

    return 0;
}

/* static  */int cambus_read_id(uint8_t addr, uint16_t *manuf_id, uint16_t *device_id)
{
    *manuf_id = 0;
    *device_id = 0;
    uint8_t tmp = 0;
    int ret = 0;
    int i2c_device = 0;
    int sccb_reg_width = 8;

    // sccb_i2c_write_byte(i2c_device, addr, 0xFF, sccb_reg_width, 0x01, 10);
    ret |= sccb_i2c_read_byte(i2c_device, addr, 0x1C, sccb_reg_width, &tmp, 100);
    *manuf_id = tmp << 8;
    ret |= sccb_i2c_read_byte(i2c_device, addr, 0x1D, sccb_reg_width, &tmp, 100);
    *manuf_id |= tmp;
    ret |= sccb_i2c_read_byte(i2c_device, addr, 0x0A, sccb_reg_width, &tmp, 100);
    *device_id = tmp << 8;
    ret |= sccb_i2c_read_byte(i2c_device, addr, 0x0B, sccb_reg_width, &tmp, 100);
    *device_id |= tmp;
    // printk("ret:%d %04x %04x\r\n",ret, *manuf_id, *device_id);
    return ret;
}

/* static  */int cambus_read16_id(uint8_t addr, uint16_t *manuf_id, uint16_t *device_id)
{
    *manuf_id = 0;
    *device_id = 0;
    uint8_t tmp = 0;
    int ret = 0;
    int i2c_device = 0;
    int sccb_reg_width = 16;

    //TODO: 0x300A 0x300B maybe just for OV3660
    // sccb_i2c_write_byte(i2c_device, addr, 0xFF, sccb_reg_width, 0x01, 10);
    ret |= sccb_i2c_read_byte(i2c_device, addr, 0x300A, sccb_reg_width, &tmp, 100);
    if (ret != 0)
        return ret;
    *device_id = tmp << 8;
    ret |= sccb_i2c_read_byte(i2c_device, addr, 0x300B, sccb_reg_width, &tmp, 100);
    *device_id |= tmp;
    // printk("ret:%d %04x %04x\r\n",ret, *manuf_id, *device_id);
    return ret;
}

static int sensor_run(int enable)
{
    if (enable)
    {
        dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
        plic_irq_enable(IRQN_DVP_INTERRUPT);
        dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
    }
    else
    {
        plic_irq_disable(IRQN_DVP_INTERRUPT);
        dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
        dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
    }

    return 0;
}

static int sensor_irq(void *ctx)
{
    if (g_dvp_finish_flag == 1)
    {
        dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    }

    // sensor_t *sensor = ctx;
    if (dvp_get_interrupt(DVP_STS_FRAME_FINISH))
    { //frame end
        dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
        g_dvp_finish_flag = 1;
    }
    else
    {                               //frame start
        if (g_dvp_finish_flag == 0) //only we finish the convert, do transmit again
            dvp_start_convert();    //so we need deal img ontime, or skip one framebefore next
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }
    //rt_kprintf("D\n");
    return 0;
}

int cambus_init(omvhal_cambus_t *bus)
{
    /* Init DVP IO map and function settings */
    fpioa_set_function(BSP_CAMERA_CMOS_RST_PIN, FUNC_CMOS_RST);
    fpioa_set_function(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_CMOS_PWDN);
    fpioa_set_function(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_CMOS_XCLK);
    fpioa_set_function(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_CMOS_VSYNC);
    fpioa_set_function(BSP_CAMERA_CMOS_HREF_PIN, FUNC_CMOS_HREF);
    fpioa_set_function(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_CMOS_PCLK);
    fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_SCCB_SCLK);
    fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN, FUNC_SCCB_SDA);

    sysctl_set_spi0_dvp_data(1);

    bus->reg_len = 8;
    dvp_init(bus->reg_len);

    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);

    //dvp_set_image_size(320, 240);
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, sensor_irq, (void *)&bus);
    //plic_irq_enable(IRQN_DVP_INTERRUPT);
    dvp_disable_auto();
    return 0;
}

int cambus_deinit(omvhal_cambus_t *bus)
{

    return 0;
}

int cambus_scan(omvhal_cambus_t *bus)
{
    uint16_t manuf_id = 0;
    uint16_t device_id = 0;

    bus->reg_len = 8;
    for (uint8_t addr = 0x08; addr <= 0x7f; addr++)
    {
        if (cambus_read_id(addr, &manuf_id, &device_id) != 0)
            continue;
        if (device_id != 0 && device_id != 0xffff)
        {
            return addr;
        }
    }

    bus->reg_len = 16;
    for (uint8_t addr = 0x08; addr <= 0x7f; addr++)
    {
        if (cambus_read16_id(addr, &manuf_id, &device_id) != 0)
            continue;
        if (device_id != 0 && device_id != 0xffff)
        {
            return addr;
        }
    }

    return 0;
}
// GC0328
int cambus_init_gc0328(omvhal_cambus_t *bus)
{
    /* Init DVP IO map and function settings (gc0328)*/
    fpioa_set_function(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_CMOS_PWDN);
    // fpioa_set_function(BSP_CAMERA_CMOS_PWDN_PIN_GC0328, FUNC_CMOS_PWDN);
    fpioa_set_function(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_CMOS_XCLK);
    fpioa_set_function(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_CMOS_VSYNC);
    fpioa_set_function(BSP_CAMERA_CMOS_HREF_PIN, FUNC_CMOS_HREF);
    fpioa_set_function(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_CMOS_PCLK);
    // fpioa_set_function(10, FUNC_I2C0_SCLK);
    sccb_i2c_switch(1);
    // fpioa_set_function(10, FUNC_SCCB_SCLK);
    // fpioa_set_function(9, FUNC_SCCB_SDA);

    sysctl_set_spi0_dvp_data(1);

    bus->reg_len = 8;
    dvp_init(bus->reg_len);

    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);

    //dvp_set_image_size(320, 240);
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, sensor_irq, (void *)&bus);
    //plic_irq_enable(IRQN_DVP_INTERRUPT);
    dvp_disable_auto();
    return 0;
}

int cambus_scan_gc0328()
{
    uint8_t device_id = 0;
    sccb_i2c_read_byte0(0, GC0328_ADDR, 0xf0, 8, &device_id, 10);
    if (device_id != 0x9d)
    {
        return 0;
    }
    return device_id;
}

int cambus_deinit_gc0328()
{
    fpioa_set_function(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_HREF_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN,  FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_SCCB_SDA0_PIN, FUNC_RESV0);
    return 0;
}
//GC0328
int cambus_init_gc0308(omvhal_cambus_t *bus)
{
   /* Init DVP IO map and function settings */
    fpioa_set_function(BSP_CAMERA_CMOS_RST_PIN, FUNC_CMOS_RST);
    fpioa_set_function(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_CMOS_PWDN);
    fpioa_set_function(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_CMOS_XCLK);
    fpioa_set_function(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_CMOS_VSYNC);
    fpioa_set_function(BSP_CAMERA_CMOS_HREF_PIN, FUNC_CMOS_HREF);
    fpioa_set_function(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_CMOS_PCLK);
    // fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_SCCB_SCLK);
    // fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN, FUNC_SCCB_SDA);
    fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN, FUNC_I2C1_SDA);
    fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_I2C1_SCLK);
    i2c_init(1, GC0308_ADDR >> 1, 7, 100000);
    sysctl_set_spi0_dvp_data(1);

    bus->reg_len = 8;
    dvp_init(bus->reg_len);

    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);

    //dvp_set_image_size(320, 240);
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, sensor_irq, (void *)&bus);
    //plic_irq_enable(IRQN_DVP_INTERRUPT);
    dvp_disable_auto();
    return 0;
}

int cambus_scan_gc0308()
{
    uint8_t device_id = 0;
    // sccb_i2c_read_byte0(0, GC0328_ADDR, 0xf0, 8, &device_id, 10);
    device_id = gc0308_rd_reg(1, 0x00);
    if (device_id != 0x9b)
    {
        return 0;
    }
    return device_id;
}

int cambus_deinit_gc0308()
{
    fpioa_set_function(BSP_CAMERA_CMOS_PWDN_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_XCLK_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_VSYNC_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_HREF_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_CMOS_PCLK_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN,  FUNC_RESV0);
    fpioa_set_function(BSP_CAMERA_SCCB_SDA0_PIN, FUNC_RESV0);
    return 0;
}

int cambus_gencall(omvhal_cambus_t *bus, uint8_t cmd)
{

    return 0;
}

int cambus_readb(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t *reg_data)
{
    int ret = 0;

    ret = sccb_i2c_read_byte(0, slv_addr, reg_addr, bus->reg_len, reg_data, 10);

    return ret;
}

int cambus_writeb(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data)
{
    int ret = 0;

    sccb_i2c_write_byte(0, slv_addr, reg_addr, bus->reg_len, reg_data, 10);

    return ret;
}

int cambus_readb2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t *reg_data)
{
    int ret = 0;

    sccb_i2c_read_byte(0, slv_addr, reg_addr, bus->reg_len, reg_data, 10);
    if (0xff == *reg_data)
        ret = -1;

    return ret;
}

int cambus_writeb2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t reg_data)
{
    int ret = 0;

    sccb_i2c_write_byte(0, slv_addr, reg_addr, bus->reg_len, reg_data, 10);

    return ret;
}

int cambus_readw(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint16_t *reg_data)
{
    int ret = 0;

    return ret;
}

int cambus_writew(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint16_t reg_data)
{
    int ret = 0;

    return ret;
}

int cambus_readw2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint16_t *reg_data)
{
    int ret = 0;

    return ret;
}

int cambus_writew2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint16_t reg_data)
{
    int ret = 0;

    return ret;
}

int cambus_read_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t *buf, int len)
{

    return 0;
}

int cambus_write_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t *buf, int len)
{

    return 0;
}

int cambus_readw_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t *buf, int len)
{

    return 0;
}

int cambus_writew_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t *buf, int len)
{

    return 0;
}

void cambus_dcmi_pwdn(omvhal_cambus_t *bus, int level)
{
    if (level)
    {
        dvp->cmos_cfg |= DVP_CMOS_POWER_DOWN;
    }
    else
    {
        dvp->cmos_cfg &= ~DVP_CMOS_POWER_DOWN;
    }
}

void cambus_dcmi_reset(omvhal_cambus_t *bus, int level)
{
    if (level)
    {
        dvp->cmos_cfg |= DVP_CMOS_RESET;
    }
    else
    {
        dvp->cmos_cfg &= ~DVP_CMOS_RESET;
    }
}

void cambus_dcmi_abort(omvhal_cambus_t *bus)
{
}

int cambus_set_pixformat(omvhal_cambus_t *bus, int *pixformat)
{
    int ret = 0;

    switch (*pixformat)
    {
    case PIXFORMAT_RGB565:
    {
        dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    }break;
    case PIXFORMAT_YUV422:
    {
        dvp_set_image_format(DVP_CFG_YUV_FORMAT);
        *pixformat = PIXFORMAT_RGB565;
    }break;
    case PIXFORMAT_GRAYSCALE:
    {
        dvp_set_image_format(DVP_CFG_Y_FORMAT);
    }break;
    default:
    {
        ret = -1;
    }break;
    }

    bus->pixformat = *pixformat;

    return ret;
}

int cambus_set_image_size(omvhal_cambus_t *bus, uint32_t width, uint32_t height)
{
    dvp_set_image_size(width, height);
    bus->width = width;
    bus->height = height;

    return 0;
}

void _ndelay(uint32_t ns)
{
    uint32_t i;

    while (ns && ns--)
    {
        for (i = 0; i < 25; i++)
            __asm__ __volatile__("nop");
    }
}

int cambus_read_pixels(omvhal_cambus_t *bus, void *buf, int timeout_ms)
{
    uint32_t start;
    uint32_t ai = (uint32_t)buf + OMV_RAW_BUF_SIZE;
    uint32_t step = bus->width * bus->height;

    g_dvp_finish_flag = 0;
    dvp_set_display_addr((uint32_t)buf);
    dvp_set_ai_addr(ai, ai + step, ai + (2 * step));
    sensor_run(1);

    start = rt_tick_get();
    while (g_dvp_finish_flag == 0)
    {
        _ndelay(50);
        if ((rt_tick_get() - start) > rt_tick_from_millisecond(timeout_ms)) //wait for 30ms
            return -1;
    }

    if (bus->pixformat == PIXFORMAT_GRAYSCALE)
    {
        memcpy(buf, (void*)((uint32_t)buf+OMV_RAW_BUF_SIZE), bus->width * bus->height);
    }
    else
    {
        uint16_t tmp, *pix = (uint16_t*)buf;
        int i;

        for (i = 0; i < (bus->width * bus->height);)
        {
            tmp = pix[i];
            pix[i] = pix[i + 1];
            pix[i + 1] = tmp;
            i += 2;
        }
    }

    return 0;
}

void cambus_set_freq(omvhal_cambus_t *bus, int clkid, uint32_t rate)
{
    if (clkid == 0)
    {
        dvp_set_xclk_rate(rate);
    }
}

void cambus_sensor_pixformat_modify(omvhal_cambus_t *bus, int *format)
{
    if (*format == PIXFORMAT_GRAYSCALE)
    {
        *format = PIXFORMAT_YUV422;
    }
}
