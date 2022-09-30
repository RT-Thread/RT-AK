#ifndef __DRV_OV2640__
#define __DRV_OV2640__
#include "dvp.h"
#include "fpioa.h"
#include "i2c.h"
#include "stdio.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <rtthread.h>
#include "syslog.h"
#include <stdint.h>
#define DBG_TAG  "LCD"
#define DBG_LVL  DBG_WARNING
#include <rtdbg.h>

#define OV2640_ADDR         0x60

static struct rt_device _ov2640;

const uint8_t ov2640_config[][2]=
{
    {0xff, 0x01},
    {0x12, 0x80},
    {0xff, 0x00},
    {0x2c, 0xff},
    {0x2e, 0xdf},
    {0xff, 0x01},
    {0x3c, 0x32},
    {0x11, 0x00},
    {0x09, 0x02},
    {0x04, 0xd8},
    {0x13, 0xe5},
    {0x14, 0x48},
    {0x2c, 0x0c},
    {0x33, 0x78},
    {0x3a, 0x33},
    {0x3b, 0xfb},
    {0x3e, 0x00},
    {0x43, 0x11},
    {0x16, 0x10},
    {0x39, 0x92},
    {0x35, 0xda},
    {0x22, 0x1a},
    {0x37, 0xc3},
    {0x23, 0x00},
    {0x34, 0xc0},
    {0x36, 0x1a},
    {0x06, 0x88},
    {0x07, 0xc0},
    {0x0d, 0x87},
    {0x0e, 0x41},
    {0x4c, 0x00},
    {0x48, 0x00},
    {0x5b, 0x00},
    {0x42, 0x03},
    {0x4a, 0x81},
    {0x21, 0x99},
    {0x24, 0x40},
    {0x25, 0x38},
    {0x26, 0x82},
    {0x5c, 0x00},
    {0x63, 0x00},
    {0x46, 0x22},
    {0x0c, 0x3c},
    {0x61, 0x70},
    {0x62, 0x80},
    {0x7c, 0x05},
    {0x20, 0x80},
    {0x28, 0x30},
    {0x6c, 0x00},
    {0x6d, 0x80},
    {0x6e, 0x00},
    {0x70, 0x02},
    {0x71, 0x94},
    {0x73, 0xc1},
    {0x3d, 0x34},
    {0x5a, 0x57},
    {0x12, 0x40},
    {0x17, 0x11},
    {0x18, 0x43},
    {0x19, 0x00},
    {0x1a, 0x4b},
    {0x32, 0x09},
    {0x37, 0xc0},
    {0x4f, 0xca},
    {0x50, 0xa8},
    {0x5a, 0x23},
    {0x6d, 0x00},
    {0x3d, 0x38},
    {0xff, 0x00},
    {0xe5, 0x7f},
    {0xf9, 0xc0},
    {0x41, 0x24},
    {0xe0, 0x14},
    {0x76, 0xff},
    {0x33, 0xa0},
    {0x42, 0x20},
    {0x43, 0x18},
    {0x4c, 0x00},
    {0x87, 0xd5},
    {0x88, 0x3f},
    {0xd7, 0x03},
    {0xd9, 0x10},
    {0xd3, 0x82},
    {0xc8, 0x08},
    {0xc9, 0x80},
    {0x7c, 0x00},
    {0x7d, 0x00},
    {0x7c, 0x03},
    {0x7d, 0x48},
    {0x7d, 0x48},
    {0x7c, 0x08},
    {0x7d, 0x20},
    {0x7d, 0x10},
    {0x7d, 0x0e},
    {0x90, 0x00},
    {0x91, 0x0e},
    {0x91, 0x1a},
    {0x91, 0x31},
    {0x91, 0x5a},
    {0x91, 0x69},
    {0x91, 0x75},
    {0x91, 0x7e},
    {0x91, 0x88},
    {0x91, 0x8f},
    {0x91, 0x96},
    {0x91, 0xa3},
    {0x91, 0xaf},
    {0x91, 0xc4},
    {0x91, 0xd7},
    {0x91, 0xe8},
    {0x91, 0x20},
    {0x92, 0x00},
    {0x93, 0x06},
    {0x93, 0xe3},
    {0x93, 0x05},
    {0x93, 0x05},
    {0x93, 0x00},
    {0x93, 0x04},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x93, 0x00},
    {0x96, 0x00},
    {0x97, 0x08},
    {0x97, 0x19},
    {0x97, 0x02},
    {0x97, 0x0c},
    {0x97, 0x24},
    {0x97, 0x30},
    {0x97, 0x28},
    {0x97, 0x26},
    {0x97, 0x02},
    {0x97, 0x98},
    {0x97, 0x80},
    {0x97, 0x00},
    {0x97, 0x00},
    {0xc3, 0xed},
    {0xa4, 0x00},
    {0xa8, 0x00},
    {0xc5, 0x11},
    {0xc6, 0x51},
    {0xbf, 0x80},
    {0xc7, 0x10},
    {0xb6, 0x66},
    {0xb8, 0xa5},
    {0xb7, 0x64},
    {0xb9, 0x7c},
    {0xb3, 0xaf},
    {0xb4, 0x97},
    {0xb5, 0xff},
    {0xb0, 0xc5},
    {0xb1, 0x94},
    {0xb2, 0x0f},
    {0xc4, 0x5c},
    {0xc0, 0x64},
    {0xc1, 0x4b},
    {0x8c, 0x00},
    {0x86, 0x3d},
    {0x50, 0x00},
    {0x51, 0xc8},
    {0x52, 0x96},
    {0x53, 0x00},
    {0x54, 0x00},
    {0x55, 0x00},
    {0x5a, 0xc8},
    {0x5b, 0x96},
    {0x5c, 0x00},
    {0xd3, 0x02},
    {0xc3, 0xed},
    {0x7f, 0x00},
    {0xda, 0x08},
    {0xe5, 0x1f},
    {0xe1, 0x67},
    {0xe0, 0x00},
    {0xdd, 0x7f},
    {0x05, 0x00},
    {0xff, 0x00},
    {0xe0, 0x04},
    {0x5a, 0x50},
    {0x5b, 0x3c},
    {0x5c, 0x00},
    {0xe0, 0x00},
    {0x00, 0x00}
};

static int ov2640_read_id(uint16_t *manuf_id, uint16_t *device_id)
{
    dvp_sccb_send_data(OV2640_ADDR, 0xFF, 0x01);
    *manuf_id = (dvp_sccb_receive_data(OV2640_ADDR, 0x1C) << 8) | dvp_sccb_receive_data(OV2640_ADDR, 0x1D);
    *device_id = (dvp_sccb_receive_data(OV2640_ADDR, 0x0A) << 8) | dvp_sccb_receive_data(OV2640_ADDR, 0x0B);
    return 0;
}

static rt_size_t _read_size;
static int dvp_irq_cb(void *ctx) {
    /* 读取DVP中断状态，如果完成则刷新显示地址的数据，并清除中断标志，否则读取摄像头数据*/
    if (dvp_get_interrupt(DVP_STS_FRAME_FINISH)) {
        dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
        dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
        if(((rt_device_t)ctx)->rx_indicate != NULL){
            ((rt_device_t)ctx)->rx_indicate((rt_device_t)ctx, _read_size);
        }
    } else {
        dvp_start_convert();
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }
    return 0;
}

static int ov2640_init(void)
{
    uint16_t v_manuf_id;
    uint16_t v_device_id;
    ov2640_read_id(&v_manuf_id, &v_device_id);
    printf("manuf_id:0x%04x,device_id:0x%04x\n", v_manuf_id, v_device_id);
    uint8_t chip_id = 0x00;
    chip_id = dvp_sccb_receive_data(OV2640_ADDR, 0x0A);
    rt_kprintf("ov2640 chip_id:0x%x\n",chip_id);
    uint16_t index = 0;
    for (index = 0; ov2640_config[index][0]; index++)
        dvp_sccb_send_data(OV2640_ADDR, ov2640_config[index][0], ov2640_config[index][1]);
    
    return 0;
}

rt_err_t  drv_ov2640_init(rt_device_t dev){
    
    dvp_init(8);  /* DVP初始化，设置sccb的寄存器长度为8bit */
    dvp_set_xclk_rate(24000000);  /* 设置输入时钟为24000000*/
    dvp_enable_burst();  /* 使能突发传输模式 */
    dvp_set_output_enable(0, 1);  /* 关闭AI输出模式，使能显示模式 */
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);  /* 设置输出格式为RGB */
    dvp_set_image_size(320, 240);  /* 设置输出像素大小为320*240 */
    ov2640_init();  /* 摄像头初始化 */
    return RT_EOK;
}

rt_err_t  drv_ov2640_open(rt_device_t dev, rt_uint16_t oflag){
      /* 设置图像开始和结束中断状态，使能或禁用 */
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();  /* 禁用自动接收图像模式 */

    /* DVP 中断配置：中断优先级，中断回调，使能DVP中断 */
    rt_kprintf("DVP interrupt config\n");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, dvp_irq_cb, (void *)dev);
    plic_irq_enable(IRQN_DVP_INTERRUPT);
    return RT_EOK;
}

rt_err_t  drv_ov2640_close(rt_device_t dev){
    
    plic_irq_disable(IRQN_DVP_INTERRUPT);
    return RT_EOK;
}

rt_size_t drv_ov2640_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size){
    uint32_t dis_buf, ai_buf;
    dis_buf = (uint32_t) buffer;
    ai_buf = dis_buf + (320 * 240 * 2);
    dvp_set_display_addr((uint32_t)buffer);
    dvp_set_ai_addr((uint32_t)ai_buf, (uint32_t)(ai_buf + 320 * 240),
                    (uint32_t)(ai_buf + 320 * 240 * 2));
    _read_size = size;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
        
    return RT_EOK;
}

rt_size_t drv_ov2640_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size){
    /* no need */
    return RT_EOK;
}

rt_err_t  drv_ov2640_control(rt_device_t dev, int cmd, void *args){
    /* no need */
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops drv_ov2640_ops =
{
    drv_ov2640_init,
    drv_ov2640_open,
    drv_ov2640_close,
    drv_ov2640_read,
    drv_ov2640_write,
    drv_ov2640_control
};
#endif


int rt_hw_ov2640_init(void)
{
    rt_err_t ret = RT_EOK;

    _ov2640.type        = RT_Device_Class_Sensor;
    _ov2640.rx_indicate = RT_NULL;
    _ov2640.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    _ov2640.ops        = &drv_ov2640_ops;
#else
    _ov2640.init    = drv_ov2640_init;
    _ov2640.open    = drv_ov2640_open;
    _ov2640.close   = drv_ov2640_close;
    _ov2640.read    = drv_ov2640_read;
    _ov2640.write   = drv_ov2640_write;
    _ov2640.control = drv_ov2640_control;
#endif
    _ov2640.user_data = RT_NULL;

    ret = rt_device_register(&_ov2640, "ov2640", RT_DEVICE_FLAG_INT_RX);
    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_ov2640_init);

#endif
