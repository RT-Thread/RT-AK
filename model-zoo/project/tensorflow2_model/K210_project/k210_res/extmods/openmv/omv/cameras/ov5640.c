/*
 * This file is part of the OpenMV project.
 *
 * Copyright (c) 2013-2019 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2019 Kwabena W. Agyeman <kwagyeman@openmv.io>
 *
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * OV5640 driver.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cambus.h"
#include "ov5640.h"
#include "ov5640_regs.h"
#include "systick.h"
#include "omv_boardconfig.h"

#if (OMV_ENABLE_OV5640 == 1)

#define BLANK_LINES             8
#define DUMMY_LINES             6

#define BLANK_COLUMNS           0
#define DUMMY_COLUMNS           16

#define SENSOR_WIDTH            2624
#define SENSOR_HEIGHT           1964

#define ACTIVE_SENSOR_WIDTH     (SENSOR_WIDTH - BLANK_COLUMNS - (2 * DUMMY_COLUMNS))
#define ACTIVE_SENSOR_HEIGHT    (SENSOR_HEIGHT - BLANK_LINES - (2 * DUMMY_LINES))

#define DUMMY_WIDTH_BUFFER      16
#define DUMMY_HEIGHT_BUFFER     8

#define HSYNC_TIME              252
#define VYSNC_TIME              24

static int16_t readout_x = 0;
static int16_t readout_y = 0;

static uint16_t readout_w = ACTIVE_SENSOR_WIDTH;
static uint16_t readout_h = ACTIVE_SENSOR_HEIGHT;

static uint16_t hts_target = 0;

#if 0
static const uint8_t default_regs[][3] = {

// https://github.com/ArduCAM/Arduino/blob/master/ArduCAM/ov5640_regs.h

    { 0x47, 0x40, 0x21 },
    { 0x40, 0x50, 0x6e },
    { 0x40, 0x51, 0x8f },
    { 0x30, 0x08, 0x42 },
    { 0x31, 0x03, 0x03 },
    { 0x30, 0x17, 0xff }, // { 0x30, 0x17, 0x7f },
    { 0x30, 0x18, 0xff },
    { 0x30, 0x2c, 0x02 },
    { 0x31, 0x08, 0x01 },
    { 0x36, 0x30, 0x2e },
    { 0x36, 0x32, 0xe2 },
    { 0x36, 0x33, 0x23 },
    { 0x36, 0x21, 0xe0 },
    { 0x37, 0x04, 0xa0 },
    { 0x37, 0x03, 0x5a },
    { 0x37, 0x15, 0x78 },
    { 0x37, 0x17, 0x01 },
    { 0x37, 0x0b, 0x60 },
    { 0x37, 0x05, 0x1a },
    { 0x39, 0x05, 0x02 },
    { 0x39, 0x06, 0x10 },
    { 0x39, 0x01, 0x0a },
    { 0x37, 0x31, 0x12 },
    { 0x36, 0x00, 0x08 },
    { 0x36, 0x01, 0x33 },
    { 0x30, 0x2d, 0x60 },
    { 0x36, 0x20, 0x52 },
    { 0x37, 0x1b, 0x20 },
    { 0x47, 0x1c, 0x50 },
    { 0x3a, 0x18, 0x00 },
    { 0x3a, 0x19, 0xf8 },
    { 0x36, 0x35, 0x1c },
    { 0x36, 0x34, 0x40 },
    { 0x36, 0x22, 0x01 },
    { 0x3c, 0x04, 0x28 },
    { 0x3c, 0x05, 0x98 },
    { 0x3c, 0x06, 0x00 },
    { 0x3c, 0x07, 0x08 },
    { 0x3c, 0x08, 0x00 },
    { 0x3c, 0x09, 0x1c },
    { 0x3c, 0x0a, 0x9c },
    { 0x3c, 0x0b, 0x40 },
    { 0x38, 0x20, 0x47 }, // { 0x38, 0x20, 0x41 },
    { 0x38, 0x21, 0x01 },
    { 0x38, 0x00, 0x00 },
    { 0x38, 0x01, 0x00 },
    { 0x38, 0x02, 0x00 },
    { 0x38, 0x03, 0x04 },
    { 0x38, 0x04, 0x0a },
    { 0x38, 0x05, 0x3f },
    { 0x38, 0x06, 0x07 },
    { 0x38, 0x07, 0x9b },
    { 0x38, 0x08, 0x05 },
    { 0x38, 0x09, 0x00 },
    { 0x38, 0x0a, 0x03 },
    { 0x38, 0x0b, 0xc0 },
    { 0x38, 0x10, 0x00 },
    { 0x38, 0x11, 0x10 },
    { 0x38, 0x12, 0x00 },
    { 0x38, 0x13, 0x06 },
    { 0x38, 0x14, 0x31 },
    { 0x38, 0x15, 0x31 },
    { 0x30, 0x34, 0x1a },
    { 0x30, 0x35, 0x11 }, // { 0x30, 0x35, 0x21 },
    { 0x30, 0x36, 0x64 }, // { 0x30, 0x36, 0x46 },
    { 0x30, 0x37, 0x13 },
    { 0x30, 0x38, 0x00 },
    { 0x30, 0x39, 0x00 },
    { 0x38, 0x0c, 0x07 },
    { 0x38, 0x0d, 0x68 },
    { 0x38, 0x0e, 0x03 },
    { 0x38, 0x0f, 0xd8 },
    { 0x3c, 0x01, 0xb4 },
    { 0x3c, 0x00, 0x04 },
    { 0x3a, 0x08, 0x00 },
    { 0x3a, 0x09, 0x93 },
    { 0x3a, 0x0e, 0x06 },
    { 0x3a, 0x0a, 0x00 },
    { 0x3a, 0x0b, 0x7b },
    { 0x3a, 0x0d, 0x08 },
    { 0x3a, 0x00, 0x38 }, // { 0x3a, 0x00, 0x3c },
    { 0x3a, 0x02, 0x05 },
    { 0x3a, 0x03, 0xc4 },
    { 0x3a, 0x14, 0x05 },
    { 0x3a, 0x15, 0xc4 },
    { 0x36, 0x18, 0x00 },
    { 0x36, 0x12, 0x29 },
    { 0x37, 0x08, 0x64 },
    { 0x37, 0x09, 0x52 },
    { 0x37, 0x0c, 0x03 },
    { 0x40, 0x01, 0x02 },
    { 0x40, 0x04, 0x02 },
    { 0x30, 0x00, 0x00 },
    { 0x30, 0x02, 0x1c },
    { 0x30, 0x04, 0xff },
    { 0x30, 0x06, 0xc3 },
    { 0x30, 0x0e, 0x58 },
    { 0x30, 0x2e, 0x00 },
    { 0x43, 0x00, 0x30 },
    { 0x50, 0x1f, 0x00 },
    { 0x47, 0x13, 0x04 }, // { 0x47, 0x13, 0x03 },
    { 0x44, 0x07, 0x04 },
    { 0x46, 0x0b, 0x35 },
    { 0x46, 0x0c, 0x22 },
    { 0x38, 0x24, 0x02 }, // { 0x38, 0x24, 0x01 },
    { 0x50, 0x01, 0xa3 },
    { 0x34, 0x06, 0x01 },
    { 0x34, 0x00, 0x06 },
    { 0x34, 0x01, 0x80 },
    { 0x34, 0x02, 0x04 },
    { 0x34, 0x03, 0x00 },
    { 0x34, 0x04, 0x06 },
    { 0x34, 0x05, 0x00 },
    { 0x51, 0x80, 0xff },
    { 0x51, 0x81, 0xf2 },
    { 0x51, 0x82, 0x00 },
    { 0x51, 0x83, 0x14 },
    { 0x51, 0x84, 0x25 },
    { 0x51, 0x85, 0x24 },
    { 0x51, 0x86, 0x16 },
    { 0x51, 0x87, 0x16 },
    { 0x51, 0x88, 0x16 },
    { 0x51, 0x89, 0x62 },
    { 0x51, 0x8a, 0x62 },
    { 0x51, 0x8b, 0xf0 },
    { 0x51, 0x8c, 0xb2 },
    { 0x51, 0x8d, 0x50 },
    { 0x51, 0x8e, 0x30 },
    { 0x51, 0x8f, 0x30 },
    { 0x51, 0x90, 0x50 },
    { 0x51, 0x91, 0xf8 },
    { 0x51, 0x92, 0x04 },
    { 0x51, 0x93, 0x70 },
    { 0x51, 0x94, 0xf0 },
    { 0x51, 0x95, 0xf0 },
    { 0x51, 0x96, 0x03 },
    { 0x51, 0x97, 0x01 },
    { 0x51, 0x98, 0x04 },
    { 0x51, 0x99, 0x12 },
    { 0x51, 0x9a, 0x04 },
    { 0x51, 0x9b, 0x00 },
    { 0x51, 0x9c, 0x06 },
    { 0x51, 0x9d, 0x82 },
    { 0x51, 0x9e, 0x38 },
    { 0x53, 0x81, 0x1e },
    { 0x53, 0x82, 0x5b },
    { 0x53, 0x83, 0x14 },
    { 0x53, 0x84, 0x06 },
    { 0x53, 0x85, 0x82 },
    { 0x53, 0x86, 0x88 },
    { 0x53, 0x87, 0x7c },
    { 0x53, 0x88, 0x60 },
    { 0x53, 0x89, 0x1c },
    { 0x53, 0x8a, 0x01 },
    { 0x53, 0x8b, 0x98 },
    { 0x53, 0x00, 0x08 },
    { 0x53, 0x01, 0x30 },
    { 0x53, 0x02, 0x3f },
    { 0x53, 0x03, 0x10 },
    { 0x53, 0x04, 0x08 },
    { 0x53, 0x05, 0x30 },
    { 0x53, 0x06, 0x18 },
    { 0x53, 0x07, 0x28 },
    { 0x53, 0x09, 0x08 },
    { 0x53, 0x0a, 0x30 },
    { 0x53, 0x0b, 0x04 },
    { 0x53, 0x0c, 0x06 },
    { 0x54, 0x80, 0x01 },
    { 0x54, 0x81, 0x06 },
    { 0x54, 0x82, 0x12 },
    { 0x54, 0x83, 0x24 },
    { 0x54, 0x84, 0x4a },
    { 0x54, 0x85, 0x58 },
    { 0x54, 0x86, 0x65 },
    { 0x54, 0x87, 0x72 },
    { 0x54, 0x88, 0x7d },
    { 0x54, 0x89, 0x88 },
    { 0x54, 0x8a, 0x92 },
    { 0x54, 0x8b, 0xa3 },
    { 0x54, 0x8c, 0xb2 },
    { 0x54, 0x8d, 0xc8 },
    { 0x54, 0x8e, 0xdd },
    { 0x54, 0x8f, 0xf0 },
    { 0x54, 0x90, 0x15 },
    { 0x55, 0x80, 0x06 },
    { 0x55, 0x83, 0x40 },
    { 0x55, 0x84, 0x20 },
    { 0x55, 0x89, 0x10 },
    { 0x55, 0x8a, 0x00 },
    { 0x55, 0x8b, 0xf8 },
    { 0x50, 0x00, 0x27 }, // { 0x50, 0x00, 0xa7 },
    { 0x58, 0x00, 0x20 },
    { 0x58, 0x01, 0x19 },
    { 0x58, 0x02, 0x17 },
    { 0x58, 0x03, 0x16 },
    { 0x58, 0x04, 0x18 },
    { 0x58, 0x05, 0x21 },
    { 0x58, 0x06, 0x0F },
    { 0x58, 0x07, 0x0A },
    { 0x58, 0x08, 0x07 },
    { 0x58, 0x09, 0x07 },
    { 0x58, 0x0a, 0x0A },
    { 0x58, 0x0b, 0x0C },
    { 0x58, 0x0c, 0x0A },
    { 0x58, 0x0d, 0x03 },
    { 0x58, 0x0e, 0x01 },
    { 0x58, 0x0f, 0x01 },
    { 0x58, 0x10, 0x03 },
    { 0x58, 0x11, 0x09 },
    { 0x58, 0x12, 0x0A },
    { 0x58, 0x13, 0x03 },
    { 0x58, 0x14, 0x01 },
    { 0x58, 0x15, 0x01 },
    { 0x58, 0x16, 0x03 },
    { 0x58, 0x17, 0x08 },
    { 0x58, 0x18, 0x10 },
    { 0x58, 0x19, 0x0A },
    { 0x58, 0x1a, 0x06 },
    { 0x58, 0x1b, 0x06 },
    { 0x58, 0x1c, 0x08 },
    { 0x58, 0x1d, 0x0E },
    { 0x58, 0x1e, 0x22 },
    { 0x58, 0x1f, 0x18 },
    { 0x58, 0x20, 0x13 },
    { 0x58, 0x21, 0x12 },
    { 0x58, 0x22, 0x16 },
    { 0x58, 0x23, 0x1E },
    { 0x58, 0x24, 0x64 },
    { 0x58, 0x25, 0x2A },
    { 0x58, 0x26, 0x2C },
    { 0x58, 0x27, 0x2A },
    { 0x58, 0x28, 0x46 },
    { 0x58, 0x29, 0x2A },
    { 0x58, 0x2a, 0x26 },
    { 0x58, 0x2b, 0x24 },
    { 0x58, 0x2c, 0x26 },
    { 0x58, 0x2d, 0x2A },
    { 0x58, 0x2e, 0x28 },
    { 0x58, 0x2f, 0x42 },
    { 0x58, 0x30, 0x40 },
    { 0x58, 0x31, 0x42 },
    { 0x58, 0x32, 0x08 },
    { 0x58, 0x33, 0x28 },
    { 0x58, 0x34, 0x26 },
    { 0x58, 0x35, 0x24 },
    { 0x58, 0x36, 0x26 },
    { 0x58, 0x37, 0x2A },
    { 0x58, 0x38, 0x44 },
    { 0x58, 0x39, 0x4A },
    { 0x58, 0x3a, 0x2C },
    { 0x58, 0x3b, 0x2a },
    { 0x58, 0x3c, 0x46 },
    { 0x58, 0x3d, 0xCE },
    { 0x56, 0x88, 0x11 }, // { 0x56, 0x88, 0x22 },
    { 0x56, 0x89, 0x11 }, // { 0x56, 0x89, 0x22 },
    { 0x56, 0x8a, 0x11 }, // { 0x56, 0x8a, 0x42 },
    { 0x56, 0x8b, 0x11 }, // { 0x56, 0x8b, 0x24 },
    { 0x56, 0x8c, 0x11 }, // { 0x56, 0x8c, 0x42 },
    { 0x56, 0x8d, 0x11 }, // { 0x56, 0x8d, 0x24 },
    { 0x56, 0x8e, 0x11 }, // { 0x56, 0x8e, 0x22 },
    { 0x56, 0x8f, 0x11 }, // { 0x56, 0x8f, 0x22 },
    { 0x50, 0x25, 0x00 },
    { 0x3a, 0x0f, 0x42 }, // { 0x3a, 0x0f, 0x30 },
    { 0x3a, 0x10, 0x38 }, // { 0x3a, 0x10, 0x28 },
    { 0x3a, 0x1b, 0x44 }, // { 0x3a, 0x1b, 0x30 },
    { 0x3a, 0x1e, 0x36 }, // { 0x3a, 0x1e, 0x28 },
    { 0x3a, 0x11, 0x60 }, // { 0x3a, 0x11, 0x61 },
    { 0x3a, 0x1f, 0x10 },
    { 0x40, 0x05, 0x1a },
    { 0x34, 0x06, 0x00 },
    { 0x35, 0x03, 0x00 },
    { 0x30, 0x08, 0x02 },

// OpenMV Custom.

    { 0x3a, 0x02, 0x07 },
    { 0x3a, 0x03, 0xae },
    { 0x3a, 0x08, 0x01 },
    { 0x3a, 0x09, 0x27 },
    { 0x3a, 0x0a, 0x00 },
    { 0x3a, 0x0b, 0xf6 },
    { 0x3a, 0x0e, 0x06 },
    { 0x3a, 0x0d, 0x08 },
    { 0x3a, 0x14, 0x07 },
    { 0x3a, 0x15, 0xae },
    { 0x44, 0x01, 0x0d }, // | Read SRAM enable when blanking | Read SRAM at first blanking
    { 0x47, 0x23, 0x03 }, // DVP JPEG Mode456 Skip Line Number

// End.

    { 0x00, 0x00, 0x00 }
};
#else
static const uint16_t default_regs[][2] =
{
    // 24MHz input clock, 24MHz PCLK
    {0x3008, 0x42}, // software power down, bit[6]
    {0x3103, 0x03}, // system clock from PLL, bit[1]
    {0x3017, 0xff}, // FREX, Vsync, HREF, PCLK, D[9:6] output enable
    {0x3018, 0xff}, // D[5:0], GPIO[1:0] output enable
    {0x3034, 0x1a}, // MIPI 10-bit
    {0x3035, 0x41},//0x41, // PLL
    {0x3036, 0x90}, // PLL
    {0x3037, 0x13},//0x13, // PLL root divider, bit[4], PLL pre-divider, bit[3:0]
    {0x3108, 0x01}, // PCLK root divider, bit[5:4], SCLK2x root divider, bit[3:2], SCLK root divider, bit[1:0]
    {0x3630, 0x36},
    {0x3631, 0x0e},
    {0x3632, 0xe2},
    {0x3633, 0x12},
    {0x3621, 0xe0},
    {0x3704, 0xa0},
    {0x3703, 0x5a},
    {0x3715, 0x78},
    {0x3717, 0x01},
    {0x370b, 0x60},
    {0x3705, 0x1a},
    {0x3905, 0x02},
    {0x3906, 0x10},
    {0x3901, 0x0a},
    {0x3731, 0x12},
    {0x3600, 0x08}, // VCM control
    {0x3601, 0x33}, // VCM control
    {0x302d, 0x60}, // system control
    {0x3620, 0x52},
    {0x371b, 0x20},
    {0x471c, 0x50},
    {0x3a13, 0x43}, // pre-gain = 1.047x
    {0x3a18, 0x00}, // gain ceiling
    {0x3a19, 0xf8}, // gain ceiling = 15.5x
    {0x3635, 0x13},
    {0x3636, 0x03},
    {0x3634, 0x40},
    {0x3622, 0x01},
    {0x3c01, 0x34}, // Band auto, bit[7]
    {0x3c04, 0x28}, // threshold low sum
    {0x3c05, 0x98}, // threshold high sum
    {0x3c06, 0x00}, // light meter 1 threshold[15:8]
    {0x3c07, 0x07}, // light meter 1 threshold[7:0]
    {0x3c08, 0x00}, // light meter 2 threshold[15:8]
    {0x3c09, 0x1c}, // light meter 2 threshold[7:0]
    {0x3c0a, 0x9c}, // sample number[15:8]
    {0x3c0b, 0x40}, // sample number[7:0]
    {0x3810, 0x00}, // Timing Hoffset[11:8]
    {0x3811, 0x10}, // Timing Hoffset[7:0]
    {0x3812, 0x00}, // Timing Voffset[10:8]
    {0x3708, 0x64},
    {0x4001, 0x02}, // BLC start from line 2
    {0x4005, 0x1a}, // BLC always update
    {0x3000, 0x00}, // enable blocks
    {0x3004, 0xff}, // enable clocks
    {0x300e, 0x58}, // MIPI power down, DVP enable
    {0x302e, 0x00},
    {0x4300, 0x61},
    {0X501F, 0x01},
    {0x3820, 0x40}, // flip
    {0x3821, 0x00}, // mirror
    {0x3814, 0x71}, // timing X inc
    {0x3815, 0x35}, // timing Y inc
    {0x3800, 0x00}, // HS
    {0x3801, 0x00}, // HS
    {0x3802, 0x00}, // VS
    {0x3803, 0x00}, // VS
    {0x3804, 0x0a}, // HW (HE)
    {0x3805, 0x3f}, // HW (HE)
    {0x3806, 0x07}, // VH (VE)
    {0x3807, 0x9f}, // VH (VE)
    {0x3808, (320 >> 8)}, // DVPHO
    {0x3809, (320 & 0xff)}, // DVPHO
    {0x380a, (240 >> 8)}, // DVPVO
    {0x380b, (240 & 0xff)}, // DVPVO
    {0x380c, 0x07}, // HTS
    {0x380d, 0x58}, // HTS
    {0x380e, 0x01}, // VTS
    {0x380f, 0xf0}, // VTS
    {0x3810, 0x00}, // HTS
    {0x3811, 0x08}, // HTS
    {0x3812, 0x00}, // VTS
    {0x3813, 0x02}, // VTS
    {0x3618, 0x00},
    {0x3612, 0x29},
    {0x3709, 0x52},
    {0x370c, 0x03},
    {0x3a02, 0x02}, // 60Hz max exposure
    {0x3a03, 0xe0}, // 60Hz max exposure
    {0x3a14, 0x02}, // 50Hz max exposure
    {0x3a15, 0xe0}, // 50Hz max exposure
    {0x4004, 0x02}, // BLC line number
    {0x3002, 0x1c}, // reset JFIFO, SFIFO, JPG
    {0x3006, 0xc3}, // disable clock of JPEG2x, JPEG
    {0x4713, 0x03}, // JPEG mode 3
    {0x4407, 0x04}, // Quantization scale
    {0x460b, 0x37},
    {0x460c, 0x20},
    {0x4837, 0x16}, // MIPI global timing
    {0x3824, 0x04}, // PCLK manual divider
    {0x5001, 0xA3}, // SDE on, scale on, UV average off, color matrix on, AWB on
    {0x3503, 0x00}, // AEC/AGC on
    {0x440e, 0x00},
    {0x5000, 0xa7}, // Lenc on, raw gamma on, BPC on, WPC on, CIP on
    {0x3a0f, 0x30}, // stable range in high
    {0x3a10, 0x28}, // stable range in low
    {0x3a1b, 0x30}, // stable range out high
    {0x3a1e, 0x26}, // stable range out low
    {0x3a11, 0x60}, // fast zone high
    {0x3a1f, 0x14}, // fast zone low
    {0x5800, 0x23},
    {0x5801, 0x14},
    {0x5802, 0x0f},
    {0x5803, 0x0f},
    {0x5804, 0x12},
    {0x5805, 0x26},
    {0x5806, 0x0c},
    {0x5807, 0x08},
    {0x5808, 0x05},
    {0x5809, 0x05},
    {0x580a, 0x08},
    {0x580b, 0x0d},
    {0x580c, 0x08},
    {0x580d, 0x03},
    {0x580e, 0x00},
    {0x580f, 0x00},
    {0x5810, 0x03},
    {0x5811, 0x09},
    {0x5812, 0x07},
    {0x5813, 0x03},
    {0x5814, 0x00},
    {0x5815, 0x01},
    {0x5816, 0x03},
    {0x5817, 0x08},
    {0x5818, 0x0d},
    {0x5819, 0x08},
    {0x581a, 0x05},
    {0x581b, 0x06},
    {0x581c, 0x08},
    {0x581d, 0x0e},
    {0x581e, 0x29},
    {0x581f, 0x17},
    {0x5820, 0x11},
    {0x5821, 0x11},
    {0x5822, 0x15},
    {0x5823, 0x28},
    {0x5824, 0x46},
    {0x5825, 0x26},
    {0x5826, 0x08},
    {0x5827, 0x26},
    {0x5828, 0x64},
    {0x5829, 0x26},
    {0x582a, 0x24},
    {0x582b, 0x22},
    {0x582c, 0x24},
    {0x582d, 0x24},
    {0x582e, 0x06},
    {0x582f, 0x22},
    {0x5830, 0x40},
    {0x5831, 0x42},
    {0x5832, 0x24},
    {0x5833, 0x26},
    {0x5834, 0x24},
    {0x5835, 0x22},
    {0x5836, 0x22},
    {0x5837, 0x26},
    {0x5838, 0x44},
    {0x5839, 0x24},
    {0x583a, 0x26},
    {0x583b, 0x28},
    {0x583c, 0x42},
    {0x583d, 0xce}, // lenc BR offset
    {0x5180, 0xff}, // AWB B block
    {0x5181, 0xf2}, // AWB control
    {0x5182, 0x00}, // [7:4] max local counter, [3:0] max fast counter
    {0x5183, 0x14}, // AWB advanced
    {0x5184, 0x25},
    {0x5185, 0x24},
    {0x5186, 0x09},
    {0x5187, 0x09},
    {0x5188, 0x09},
    {0x5189, 0x75},
    {0x518a, 0x54},
    {0x518b, 0xe0},
    {0x518c, 0xb2},
    {0x518d, 0x42},
    {0x518e, 0x3d},
    {0x518f, 0x56},
    {0x5190, 0x46},
    {0x5191, 0xf8}, // AWB top limit
    {0x5192, 0x04}, // AWB bottom limit
    {0x5193, 0x70}, // red limit
    {0x5194, 0xf0}, // green limit
    {0x5195, 0xf0}, // blue limit
    {0x5196, 0x03}, // AWB control
    {0x5197, 0x01}, // local limit
    {0x5198, 0x04},
    {0x5199, 0x12},
    {0x519a, 0x04},
    {0x519b, 0x00},
    {0x519c, 0x06},
    {0x519d, 0x82},
    {0x519e, 0x38}, // AWB control
    {0x5480, 0x01}, // Gamma bias plus on, bit[0]
    {0x5481, 0x08},
    {0x5482, 0x14},
    {0x5483, 0x28},
    {0x5484, 0x51},
    {0x5485, 0x65},
    {0x5486, 0x71},
    {0x5487, 0x7d},
    {0x5488, 0x87},
    {0x5489, 0x91},
    {0x548a, 0x9a},
    {0x548b, 0xaa},
    {0x548c, 0xb8},
    {0x548d, 0xcd},
    {0x548e, 0xdd},
    {0x548f, 0xea},
    {0x5490, 0x1d},
    {0x5381, 0x1e}, // CMX1 for Y
    {0x5382, 0x5b}, // CMX2 for Y
    {0x5383, 0x08}, // CMX3 for Y
    {0x5384, 0x0a}, // CMX4 for U
    {0x5385, 0x7e}, // CMX5 for U
    {0x5386, 0x88}, // CMX6 for U
    {0x5387, 0x7c}, // CMX7 for V
    {0x5388, 0x6c}, // CMX8 for V
    {0x5389, 0x10}, // CMX9 for V
    {0x538a, 0x01}, // sign[9]
    {0x538b, 0x98}, // sign[8:1]
    {0x5580, 0x06}, // saturation on, bit[1]
    {0x5583, 0x40},
    {0x5584, 0x10},
    {0x5589, 0x10},
    {0x558a, 0x00},
    {0x558b, 0xf8},
    {0x501d, 0x40}, // enable manual offset of contrast
    {0x5300, 0x08}, // CIP sharpen MT threshold 1
    {0x5301, 0x30}, // CIP sharpen MT threshold 2
    {0x5302, 0x10}, // CIP sharpen MT offset 1
    {0x5303, 0x00}, // CIP sharpen MT offset 2
    {0x5304, 0x08}, // CIP DNS threshold 1
    {0x5305, 0x30}, // CIP DNS threshold 2
    {0x5306, 0x08}, // CIP DNS offset 1
    {0x5307, 0x16}, // CIP DNS offset 2
    {0x5309, 0x08}, // CIP sharpen TH threshold 1
    {0x530a, 0x30}, // CIP sharpen TH threshold 2
    {0x530b, 0x04}, // CIP sharpen TH offset 1
    {0x530c, 0x06}, // CIP sharpen TH offset 2
    {0x5025, 0x00},
    {0x3008, 0x02}, // wake up from standby, bit[6]
    {0x4740, 0X21}, //VSYNC active HIGH

    {0, 0}
};
#endif

#define NUM_BRIGHTNESS_LEVELS (9)

#define NUM_CONTRAST_LEVELS (7)
static const uint8_t contrast_regs[NUM_CONTRAST_LEVELS][1] = {
    {0x14}, /* -3 */
    {0x18}, /* -2 */
    {0x1C}, /* -1 */
    {0x00}, /* +0 */
    {0x10}, /* +1 */
    {0x18}, /* +2 */
    {0x1C}, /* +3 */
};

#define NUM_SATURATION_LEVELS (7)
static const uint8_t saturation_regs[NUM_SATURATION_LEVELS][6] = {
    {0x0c, 0x30, 0x3d, 0x3e, 0x3d, 0x01}, /* -3 */
    {0x10, 0x3d, 0x4d, 0x4e, 0x4d, 0x01}, /* -2 */
    {0x15, 0x52, 0x66, 0x68, 0x66, 0x02}, /* -1 */
    {0x1a, 0x66, 0x80, 0x82, 0x80, 0x02}, /* +0 */
    {0x1f, 0x7a, 0x9a, 0x9c, 0x9a, 0x02}, /* +1 */
    {0x24, 0x8f, 0xb3, 0xb6, 0xb3, 0x03}, /* +2 */
    {0x2b, 0xab, 0xd6, 0xda, 0xd6, 0x04}, /* +3 */
};

static int reset(sensor_t *sensor)
{
    readout_x = 0;
    readout_y = 0;

    readout_w = ACTIVE_SENSOR_WIDTH;
    readout_h = ACTIVE_SENSOR_HEIGHT;

    hts_target = 0;

    // Reset all registers
    int ret = cambus_writeb2(&sensor->cambus, sensor->slv_addr, SCCB_SYSTEM_CTRL_1, 0x11);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, SYSTEM_CTROL0, 0x82);

    // Delay 5 ms
    systick_sleep(5);
#if 0
    // Write default regsiters
    for (int i = 0; default_regs[i][0]; i++) {
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, (default_regs[i][0] << 8) | (default_regs[i][1] << 0), default_regs[i][2]);
    }
#else
    for (int i = 0; default_regs[i][0]; i++) {
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, default_regs[i][0], default_regs[i][1]);
    }
#endif

    // Delay 300 ms
    systick_sleep(300);

    return ret;
}

static int sleep(sensor_t *sensor, int enable)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, SYSTEM_CTROL0, &reg);

    if (enable) {
        reg |= 0x40;
    } else {
        reg &= ~0x40;
    }

    return cambus_writeb2(&sensor->cambus, sensor->slv_addr, SYSTEM_CTROL0, reg) | ret;
}

static int read_reg(sensor_t *sensor, uint16_t reg_addr)
{
    uint8_t reg_data;
    if (cambus_readb2(&sensor->cambus, sensor->slv_addr, reg_addr, &reg_data) != 0) {
        return -1;
    }
    return reg_data;
}

static int write_reg(sensor_t *sensor, uint16_t reg_addr, uint16_t reg_data)
{
    return cambus_writeb2(&sensor->cambus, sensor->slv_addr, reg_addr, reg_data);
}

// HTS (Horizontal Time) is the readout width plus the HSYNC_TIME time. However, if this value gets
// too low the OV5640 will crash. The minimum was determined empirically with testing...
// Additionally, when the image width gets too large we need to slow down the line transfer rate by
// increasing HTS so that DCMI_DMAConvCpltUser() can keep up with the data rate.
//
// WARNING! IF YOU CHANGE ANYTHING HERE RETEST WITH **ALL** RESOLUTIONS FOR THE AFFECTED MODE!
static int calculate_hts(sensor_t *sensor, uint16_t width)
{
    uint16_t hts = hts_target;

    if ((sensor->pixformat == PIXFORMAT_GRAYSCALE) || (sensor->pixformat == PIXFORMAT_BAYER) || (sensor->pixformat == PIXFORMAT_JPEG)) {
        if (width <= 1280) hts = IM_MAX((width * 2) + 8, hts_target);
    } else {
        if (width > 640) hts = IM_MAX((width * 2) + 8, hts_target);
    }

    if (width <= 640) hts += 160; // Fix image quality at low resolutions.

    return IM_MAX(hts + HSYNC_TIME, (SENSOR_WIDTH + HSYNC_TIME) / 2); // Fix to prevent crashing.
}

// VTS (Vertical Time) is the readout height plus the VYSNC_TIME time. However, if this value gets
// too low the OV5640 will crash. The minimum was determined empirically with testing...
//
// WARNING! IF YOU CHANGE ANYTHING HERE RETEST WITH **ALL** RESOLUTIONS FOR THE AFFECTED MODE!
static int calculate_vts(sensor_t *sensor, uint16_t readout_height)
{
    return IM_MAX(readout_height + VYSNC_TIME, (SENSOR_HEIGHT + VYSNC_TIME) / 8); // Fix to prevent crashing.
}

static int set_pixformat(sensor_t *sensor, pixformat_t pixformat)
{
    uint8_t reg;
    int ret = 0;

    // Not a multiple of 8. The JPEG encoder on the OV5640 can't handle this.
    if ((pixformat == PIXFORMAT_JPEG) && ((resolution[sensor->framesize][0] % 8) || (resolution[sensor->framesize][1] % 8))) {
        return -1;
    }

    // Readout speed too fast. The DCMI_DMAConvCpltUser() line callback overhead is too much to handle the line transfer speed.
    // If we were to slow the pixclk down these resolutions would work. As of right now, the image shakes and scrolls with
    // the current line transfer speed. Note that there's an overhead to the DCMI_DMAConvCpltUser() function. It's not the
    // memory copy operation that's too slow. It's that there's too much overhead in the DCMI_DMAConvCpltUser() method
    // to even have time to start the line transfer. If it were possible to slow the line readout speed of the OV5640
    // this would enable these resolutions below. However, there's nothing in the datasheet that when modified does this.
    if (((pixformat == PIXFORMAT_GRAYSCALE) || (pixformat == PIXFORMAT_BAYER) || (pixformat == PIXFORMAT_JPEG))
    && ((sensor->framesize == FRAMESIZE_QQCIF)
    || (sensor->framesize == FRAMESIZE_QQSIF)
    || (sensor->framesize == FRAMESIZE_HQQQVGA)
    || (sensor->framesize == FRAMESIZE_HQQVGA))) {
        return -1;
    }

    switch (pixformat) {
        case PIXFORMAT_GRAYSCALE:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL, 0x10);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL_MUX, 0x00);
            break;
        case PIXFORMAT_RGB565:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL, 0x61);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL_MUX, 0x01);
            break;
        case PIXFORMAT_YUV422:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL, 0x32);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL_MUX, 0x00);
            break;
        case PIXFORMAT_BAYER:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL, 0x00);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL_MUX, 0x01);
            break;
        case PIXFORMAT_JPEG:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL, 0x30);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, FORMAT_CONTROL_MUX, 0x00);
            break;
        default:
            return -1;
    }

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, (reg & 0xDF) | ((pixformat == PIXFORMAT_JPEG) ? 0x20 : 0x00));

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, SYSTEM_RESET_02, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, SYSTEM_RESET_02, (reg & 0xE3) | ((pixformat == PIXFORMAT_JPEG) ? 0x00 : 0x1C));

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, CLOCK_ENABLE_02, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, CLOCK_ENABLE_02, (reg & 0xD7) | ((pixformat == PIXFORMAT_JPEG) ? 0x28 : 0x00));

    if (hts_target) {
        uint16_t sensor_hts = calculate_hts(sensor, resolution[sensor->framesize][0]);

        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_H, sensor_hts >> 8);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_L, sensor_hts);
    }

    return ret;
}

static int set_framesize(sensor_t *sensor, framesize_t framesize)
{
    uint8_t reg;
    int ret = 0;
    uint16_t w = resolution[framesize][0];
    uint16_t h = resolution[framesize][1];
return 0;//TODO
    // Not a multiple of 8. The JPEG encoder on the OV5640 can't handle this.
    if ((sensor->pixformat == PIXFORMAT_JPEG) && ((w % 8) || (h % 8))) {
        return -1;
    }

    // Readout speed too fast. The DCMI_DMAConvCpltUser() line callback overhead is too much to handle the line transfer speed.
    // If we were to slow the pixclk down these resolutions would work. As of right now, the image shakes and scrolls with
    // the current line transfer speed. Note that there's an overhead to the DCMI_DMAConvCpltUser() function. It's not the
    // memory copy operation that's too slow. It's that there's too much overhead in the DCMI_DMAConvCpltUser() method
    // to even have time to start the line transfer. If it were possible to slow the line readout speed of the OV5640
    // this would enable these resolutions below. However, there's nothing in the datasheet that when modified does this.
    if (((sensor->pixformat == PIXFORMAT_GRAYSCALE) || (sensor->pixformat == PIXFORMAT_BAYER) || (sensor->pixformat == PIXFORMAT_JPEG))
    && ((framesize == FRAMESIZE_QQCIF)
    || (framesize == FRAMESIZE_QQSIF)
    || (framesize == FRAMESIZE_HQQQVGA)
    || (framesize == FRAMESIZE_HQQVGA))) {
        return -1;
    }

    // Generally doesn't work for anything.
    if (framesize == FRAMESIZE_QQQQVGA) {
        return -1;
    }

    // Invalid resolution.
    if ((w > ACTIVE_SENSOR_WIDTH) || (h > ACTIVE_SENSOR_HEIGHT)) {
        return -1;
    }

    // Step 0: Clamp readout settings.

    readout_w = IM_MAX(readout_w, w);
    readout_h = IM_MAX(readout_h, h);

    int readout_x_max = (ACTIVE_SENSOR_WIDTH - readout_w) / 2;
    int readout_y_max = (ACTIVE_SENSOR_HEIGHT - readout_h) / 2;
    readout_x = IM_MAX(IM_MIN(readout_x, readout_x_max), -readout_x_max);
    readout_y = IM_MAX(IM_MIN(readout_y, readout_y_max), -readout_y_max);

    // Step 1: Determine readout area and subsampling amount.

    uint16_t sensor_div = 0;

    if ((w > (readout_w / 2)) || (h > (readout_h / 2))) {
        sensor_div = 1;
    } else {
        sensor_div = 2;
    }

    // Step 2: Determine horizontal and vertical start and end points.

    uint16_t sensor_w = readout_w + DUMMY_WIDTH_BUFFER; // camera hardware needs dummy pixels to sync
    uint16_t sensor_h = readout_h + DUMMY_HEIGHT_BUFFER; // camera hardware needs dummy lines to sync

    uint16_t sensor_ws = IM_MAX(IM_MIN((((ACTIVE_SENSOR_WIDTH - sensor_w) / 4) + (readout_x / 2)) * 2, ACTIVE_SENSOR_WIDTH - sensor_w), -(DUMMY_WIDTH_BUFFER / 2)) + DUMMY_COLUMNS; // must be multiple of 2
    uint16_t sensor_we = sensor_ws + sensor_w - 1;

    uint16_t sensor_hs = IM_MAX(IM_MIN((((ACTIVE_SENSOR_HEIGHT - sensor_h) / 4) - (readout_y / 2)) * 2, ACTIVE_SENSOR_HEIGHT - sensor_h), -(DUMMY_HEIGHT_BUFFER / 2)) + DUMMY_LINES; // must be multiple of 2
    uint16_t sensor_he = sensor_hs + sensor_h - 1;

    // Step 3: Determine scaling window offset.

    float ratio = IM_MIN((readout_w / sensor_div) / ((float) w), (readout_h / sensor_div) / ((float) h));

    uint16_t w_mul = w * ratio;
    uint16_t h_mul = h * ratio;
    uint16_t x_off = ((sensor_w / sensor_div) - w_mul) / 2;
    uint16_t y_off = ((sensor_h / sensor_div) - h_mul) / 2;

    // Step 4: Compute total frame time.

    hts_target = sensor_w / sensor_div;

    uint16_t sensor_hts = calculate_hts(sensor, w);
    uint16_t sensor_vts = calculate_vts(sensor, sensor_h / sensor_div);

    uint16_t sensor_x_inc = (((sensor_div * 2) - 1) << 4) | (1 << 0); // odd[7:4]/even[3:0] pixel inc on the bayer pattern
    uint16_t sensor_y_inc = (((sensor_div * 2) - 1) << 4) | (1 << 0); // odd[7:4]/even[3:0] pixel inc on the bayer pattern

    // Step 5: Write regs.

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HS_H, sensor_ws >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HS_L, sensor_ws);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VS_H, sensor_hs >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VS_L, sensor_hs);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HW_H, sensor_we >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HW_L, sensor_we);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VH_H, sensor_he >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VH_L, sensor_he);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_DVPHO_H, w >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_DVPHO_L, w);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_DVPVO_H, h >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_DVPVO_L, h);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_H, sensor_hts >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_L, sensor_hts);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VTS_H, sensor_vts >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VTS_L, sensor_vts);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HOFFSET_H, x_off >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_HOFFSET_L, x_off);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VOFFSET_H, y_off >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VOFFSET_L, y_off);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_X_INC, sensor_x_inc);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_Y_INC, sensor_y_inc);

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_20, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_20, (reg & 0xFE) | (sensor_div > 1));

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, (reg & 0xFE) | (sensor_div > 1));

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, VFIFO_HSIZE_H, w >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, VFIFO_HSIZE_L, w);

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, VFIFO_VSIZE_H, h >> 8);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, VFIFO_VSIZE_L, h);

    return ret;
}

static int set_contrast(sensor_t *sensor, int level)
{
    int ret = 0;

    int new_level = level + (NUM_CONTRAST_LEVELS / 2);
    if (new_level < 0 || new_level >= NUM_CONTRAST_LEVELS) {
        return -1;
    }

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x03); // start group 3
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5586, (new_level + 5) << 2);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5585, contrast_regs[new_level][0]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x13); // end group 3
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0xa3); // launch group 3

    return ret;
}

static int set_brightness(sensor_t *sensor, int level)
{
    int ret = 0;

    int new_level = level + (NUM_BRIGHTNESS_LEVELS / 2);
    if (new_level < 0 || new_level >= NUM_BRIGHTNESS_LEVELS) {
        return -1;
    }

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x03); // start group 3
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5587, abs(level) << 4);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5588, (level < 0) ? 0x09 : 0x01);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x13); // end group 3
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0xa3); // launch group 3

    return ret;
}

static int set_saturation(sensor_t *sensor, int level)
{
    int ret = 0;

    int new_level = level + (NUM_SATURATION_LEVELS / 2);
    if (new_level < 0 || new_level >= NUM_SATURATION_LEVELS) {
        return -1;
    }

    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x03); // start group 3
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5581, 0x1c);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5582, 0x5a);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5583, 0x06);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5584, saturation_regs[new_level][0]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5585, saturation_regs[new_level][1]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5586, saturation_regs[new_level][2]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5587, saturation_regs[new_level][3]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5588, saturation_regs[new_level][4]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5589, saturation_regs[new_level][5]);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x558b, 0x98);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x558a, 0x01);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x13); // end group 3
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0xa3); // launch group 3

    return ret;
}

static int set_gainceiling(sensor_t *sensor, gainceiling_t gainceiling)
{
    uint8_t reg;
    int ret = 0;

    int new_gainceiling = 16 << (gainceiling + 1);
    if (new_gainceiling >= 1024) {
        return -1;
    }

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_GAIN_CEILING_H, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_GAIN_CEILING_H, (reg & 0xFC) | (new_gainceiling >> 8));
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_GAIN_CEILING_L, new_gainceiling);

    return ret;
}

static int set_quality(sensor_t *sensor, int qs)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, JPEG_CTRL07, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, JPEG_CTRL07, (reg & 0xC0) | (qs >> 2));

    return ret;
}

static int set_colorbar(sensor_t *sensor, int enable)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, PRE_ISP_TEST, &reg);
    return cambus_writeb2(&sensor->cambus, sensor->slv_addr, PRE_ISP_TEST, (reg & 0x7F) | (enable ? 0x80 : 0x00)) | ret;
}

static int set_auto_gain(sensor_t *sensor, int enable, float gain_db, float gain_db_ceiling)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_MANUAL, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_MANUAL, (reg & 0xFD) | ((enable == 0) << 1));

    if ((enable == 0) && (!isnanf(gain_db)) && (!isinff(gain_db))) {
        int gain = IM_MAX(IM_MIN(expf((gain_db / 20.0) * log(10.0)) * 16.0, 1023), 0);

        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_REAL_GAIN_H, &reg);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_REAL_GAIN_H, (reg & 0xFC) | (gain >> 8));
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_REAL_GAIN_L, gain);
    } else if ((enable != 0) && (!isnanf(gain_db_ceiling)) && (!isinff(gain_db_ceiling))) {
        int gain_ceiling = IM_MAX(IM_MIN(expf((gain_db_ceiling / 20.0) * log(10.0)) * 16.0, 1023), 0);

        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_GAIN_CEILING_H, &reg);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_GAIN_CEILING_H, (reg & 0xFC) | (gain_ceiling >> 8));
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_GAIN_CEILING_L, gain_ceiling);
    }

    return ret;
}

static int get_gain_db(sensor_t *sensor, float *gain_db)
{
    uint8_t gainh, gainl;

    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_REAL_GAIN_H, &gainh);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_REAL_GAIN_L, &gainl);

    *gain_db = 20.0 * (log((((gainh & 0x3) << 8) | gainl) / 16.0) / log(10.0));

    return ret;
}

static int set_auto_exposure(sensor_t *sensor, int enable, int exposure_us)
{
    uint8_t reg, pll, hts_h, hts_l, vts_h, vts_l;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_MANUAL, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_MANUAL, (reg & 0xFE) | ((enable == 0) << 0));

    if ((enable == 0) && (exposure_us >= 0)) {
        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, SC_PLL_CONTRL2, &pll);

        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_H, &hts_h);
        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_L, &hts_l);

        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_VTS_H, &vts_h);
        ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_VTS_L, &vts_l);

        uint16_t hts = (hts_h << 8) | hts_l;
        uint16_t vts = (vts_h << 8) | vts_l;

        // "/ 3" -> SC_PLL_CONTRL3[3:0] (pll pre-divider)
        // "/ 1" -> SC_PLL_CONTRL1[7:4] (system clock divider)
        // "/ 2" -> SC_PLL_CONTRL3[4] (pll root divider)
        // "/ 10" -> SYSTEM_CTROL0[3:0] (bit mode)
        // "/ 1" -> SYSTEM_ROOT_DIVIDER[5:4] (pclk root divider)
        int pclk_freq = ((((((OV5640_XCLK_FREQ / 3) * pll) / 1) / 2) / 10) / 1) * 2;
        int clocks_per_us = pclk_freq / 1000000;
        int exposure = IM_MAX(IM_MIN((exposure_us * clocks_per_us) / hts, 0xFFFF), 0x0000);

        int new_vts = IM_MAX(exposure, vts);

        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_EXPOSURE_0, exposure >> 12);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_EXPOSURE_1, exposure >> 4);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AEC_PK_EXPOSURE_2, exposure << 4);

        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VTS_H, new_vts >> 8);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_VTS_L, new_vts);
    }

    return ret;
}

static int get_exposure_us(sensor_t *sensor, int *exposure_us)
{
    uint8_t pll, aec_0, aec_1, aec_2, hts_h, hts_l;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, SC_PLL_CONTRL2, &pll);

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_EXPOSURE_0, &aec_0);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_EXPOSURE_1, &aec_1);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AEC_PK_EXPOSURE_2, &aec_2);

    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_H, &hts_h);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_HTS_L, &hts_l);

    uint32_t aec = ((aec_0 << 16) | (aec_1 << 8) | aec_2) >> 4;
    uint16_t hts = (hts_h << 8) | hts_l;

    // "/ 3" -> SC_PLL_CONTRL3[3:0] (pll pre-divider)
    // "/ 1" -> SC_PLL_CONTRL1[7:4] (system clock divider)
    // "/ 2" -> SC_PLL_CONTRL3[4] (pll root divider)
    // "/ 10" -> SYSTEM_CTROL0[3:0] (bit mode)
    // "/ 1" -> SYSTEM_ROOT_DIVIDER[5:4] (pclk root divider)
    int pclk_freq = ((((((OV5640_XCLK_FREQ / 3) * pll) / 1) / 2) / 10) / 1) * 2;
    int clocks_per_us = pclk_freq / 1000000;
    *exposure_us = (aec * hts) / clocks_per_us;

    return ret;
}

static int set_auto_whitebal(sensor_t *sensor, int enable, float r_gain_db, float g_gain_db, float b_gain_db)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_MANUAL_CONTROL, &reg);
    ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_MANUAL_CONTROL, (reg & 0xFE) | (enable == 0));

    if ((enable == 0) && (!isnanf(r_gain_db)) && (!isnanf(g_gain_db)) && (!isnanf(b_gain_db))
                      && (!isinff(r_gain_db)) && (!isinff(g_gain_db)) && (!isinff(b_gain_db))) {

        int r_gain = IM_MAX(IM_MIN(roundf(expf((r_gain_db / 20.0) * log(10.0))), 4095), 0);
        int g_gain = IM_MAX(IM_MIN(roundf(expf((g_gain_db / 20.0) * log(10.0))), 4095), 0);
        int b_gain = IM_MAX(IM_MIN(roundf(expf((b_gain_db / 20.0) * log(10.0))), 4095), 0);

        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_R_GAIN_H, r_gain >> 8);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_R_GAIN_L, r_gain);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_G_GAIN_H, g_gain >> 8);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_G_GAIN_L, g_gain);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_B_GAIN_H, b_gain >> 8);
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, AWB_B_GAIN_L, b_gain);
    }

    return ret;
}

static int get_rgb_gain_db(sensor_t *sensor, float *r_gain_db, float *g_gain_db, float *b_gain_db)
{
    uint8_t redh, redl, greenh, greenl, blueh, bluel;

    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_R_GAIN_H, &redh);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_R_GAIN_L, &redl);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_G_GAIN_H, &greenh);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_G_GAIN_L, &greenl);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_B_GAIN_H, &blueh);
    ret |= cambus_readb2(&sensor->cambus, sensor->slv_addr, AWB_B_GAIN_L, &bluel);

    *r_gain_db = 20.0 * (log(((redh & 0xF) << 8) | redl) / log(10.0));
    *g_gain_db = 20.0 * (log(((greenh & 0xF) << 8) | greenl) / log(10.0));
    *b_gain_db = 20.0 * (log(((blueh & 0xF) << 8) | bluel) / log(10.0));

    return ret;
}

static int set_hmirror(sensor_t *sensor, int enable)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, &reg);
    if (enable){
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, reg|0x06);
    } else {
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_21, reg&0xF9);
    }
    return ret;
}

static int set_vflip(sensor_t *sensor, int enable)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_20, &reg);
    if (!enable){
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_20, reg|0x06);
    } else {
        ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, TIMING_TC_REG_20, reg&0xF9);
    }
    return ret;
}

static int set_special_effect(sensor_t *sensor, sde_t sde)
{
    int ret = 0;

    switch (sde) {
        case SDE_NEGATIVE:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x03); // start group 3
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5580, 0x40);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5003, 0x08);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5583, 0x40); // sat U
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5584, 0x10); // sat V
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x13); // end group 3
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0xa3); // latch group 3
            break;
        case SDE_NORMAL:
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x03); // start group 3
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5580, 0x06);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5583, 0x40); // sat U
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5584, 0x10); // sat V
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x5003, 0x08);
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0x13); // end group 3
            ret |= cambus_writeb2(&sensor->cambus, sensor->slv_addr, 0x3212, 0xa3); // latch group 3
            break;
        default:
            return -1;
    }

    return ret;
}

static int set_lens_correction(sensor_t *sensor, int enable, int radi, int coef)
{
    uint8_t reg;
    int ret = cambus_readb2(&sensor->cambus, sensor->slv_addr, ISP_CONTROL_00, &reg);
    return cambus_writeb2(&sensor->cambus, sensor->slv_addr, ISP_CONTROL_00, (reg & 0x7F) | (enable ? 0x80 : 0x00)) | ret;
}

static int ioctl(sensor_t *sensor, int request, va_list ap)
{
    int ret = 0;

    switch (request) {
        case IOCTL_SET_READOUT_WINDOW: {
            int tmp_readout_x = va_arg(ap, int);
            int tmp_readout_y = va_arg(ap, int);
            int tmp_readout_w = IM_MAX(IM_MIN(va_arg(ap, int), ACTIVE_SENSOR_WIDTH), resolution[sensor->framesize][0]);
            int tmp_readout_h = IM_MAX(IM_MIN(va_arg(ap, int), ACTIVE_SENSOR_HEIGHT), resolution[sensor->framesize][1]);
            int readout_x_max = (ACTIVE_SENSOR_WIDTH - tmp_readout_w) / 2;
            int readout_y_max = (ACTIVE_SENSOR_HEIGHT - tmp_readout_h) / 2;
            tmp_readout_x = IM_MAX(IM_MIN(tmp_readout_x, readout_x_max), -readout_x_max);
            tmp_readout_y = IM_MAX(IM_MIN(tmp_readout_y, readout_y_max), -readout_y_max);
            bool changed = (tmp_readout_x != readout_x) || (tmp_readout_y != readout_y) || (tmp_readout_w != readout_w) || (tmp_readout_h != readout_h);
            readout_x = tmp_readout_x;
            readout_y = tmp_readout_y;
            readout_w = tmp_readout_w;
            readout_h = tmp_readout_h;
            if (changed && (sensor->framesize != FRAMESIZE_INVALID)) set_framesize(sensor, sensor->framesize);
            break;
        }
        case IOCTL_GET_READOUT_WINDOW: {
            *va_arg(ap, int *) = readout_x;
            *va_arg(ap, int *) = readout_y;
            *va_arg(ap, int *) = readout_w;
            *va_arg(ap, int *) = readout_h;
            break;
        }
        default: {
            ret = -1;
            break;
        }
    }

    return ret;
}

int ov5640_init(sensor_t *sensor)
{
    // Initialize sensor structure.
    sensor->gs_bpp              = 1;
    sensor->reset               = reset;
    sensor->sleep               = sleep;
    sensor->read_reg            = read_reg;
    sensor->write_reg           = write_reg;
    sensor->set_pixformat       = set_pixformat;
    sensor->set_framesize       = set_framesize;
    sensor->set_contrast        = set_contrast;
    sensor->set_brightness      = set_brightness;
    sensor->set_saturation      = set_saturation;
    sensor->set_gainceiling     = set_gainceiling;
    sensor->set_quality         = set_quality;
    sensor->set_colorbar        = set_colorbar;
    sensor->set_auto_gain       = set_auto_gain;
    sensor->get_gain_db         = get_gain_db;
    sensor->set_auto_exposure   = set_auto_exposure;
    sensor->get_exposure_us     = get_exposure_us;
    sensor->set_auto_whitebal   = set_auto_whitebal;
    sensor->get_rgb_gain_db     = get_rgb_gain_db;
    sensor->set_hmirror         = set_hmirror;
    sensor->set_vflip           = set_vflip;
    sensor->set_special_effect  = set_special_effect;
    sensor->set_lens_correction = set_lens_correction;
    sensor->ioctl               = ioctl;

    // Set sensor flags
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_VSYNC, 1);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_HSYNC, 0);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_PIXCK, 1);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_FSYNC, 0);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_JPEGE, 1);

    return 0;
}
#endif // (OMV_ENABLE_OV5640 == 1)
