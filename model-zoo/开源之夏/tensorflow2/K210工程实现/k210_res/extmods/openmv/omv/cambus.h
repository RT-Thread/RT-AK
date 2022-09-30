/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Camera bus driver.
 *
 */
#ifndef __CAMBUS_H__
#define __CAMBUS_H__

#include <stdint.h>
#include "omvhaldef.h"

int cambus_init(omvhal_cambus_t *bus);
int cambus_init_gc0328(omvhal_cambus_t *bus);
int cambus_scan_gc0328();
void open_gc0328_0();
void open_gc0328_1();
void sccb_i2c_switch(int num);
int cambus_deinit_gc0328();
int cambus_deinit(omvhal_cambus_t *bus);
int cambus_scan(omvhal_cambus_t *bus);
int cambus_gencall(omvhal_cambus_t *bus, uint8_t cmd);
int cambus_readb(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr,  uint8_t *reg_data);
int cambus_writeb(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t reg_data);
int cambus_readb2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr,  uint8_t *reg_data);
int cambus_writeb2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t reg_data);
int cambus_readw(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr,  uint16_t *reg_data);
int cambus_writew(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint16_t reg_data);
int cambus_readw2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr,  uint16_t *reg_data);
int cambus_writew2(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint16_t reg_data);
int cambus_read_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t *buf, int len);
int cambus_write_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint8_t reg_addr, uint8_t *buf, int len);
int cambus_readw_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t *buf, int len);
int cambus_writew_bytes(omvhal_cambus_t *bus, uint8_t slv_addr, uint16_t reg_addr, uint8_t *buf, int len);
int cambus_readb3( uint8_t slv_addr, uint16_t reg_addr, uint8_t *reg_data);
int cambus_writeb3(uint8_t slv_addr, uint16_t reg_addr, uint8_t reg_data);

void cambus_dcmi_pwdn(omvhal_cambus_t *bus, int level);
void cambus_dcmi_reset(omvhal_cambus_t *bus, int level);
void cambus_dcmi_abort(omvhal_cambus_t *bus);
int cambus_set_pixformat(omvhal_cambus_t *bus, int *format);
int cambus_read_pixels(omvhal_cambus_t *bus, void *buf, int timeout_ms);
int cambus_set_image_size(omvhal_cambus_t *bus, uint32_t width, uint32_t height);
void cambus_set_freq(omvhal_cambus_t *bus, int clkid, uint32_t rate);
void cambus_sensor_pixformat_modify(omvhal_cambus_t *bus, int *format);

#endif // __CAMBUS_H__
