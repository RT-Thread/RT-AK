/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
#include "sensor.h"
// #define DVP_PIN_SDA 15
// #define DVP_PIN_SDA0 0
// #define DVP_PIN_SCL 14

#define LOGE(tag, format, ...) rt_kprintf("[LOGE]" tag format, ##__VA_ARGS__)
#define LOGW(tag, format, ...) rt_kprintf("[LOGW]" tag format, ##__VA_ARGS__)
#define LOGI(tag, format, ...) rt_kprintf("[LOGI]" tag format, ##__VA_ARGS__)
#define LOGD(tag, format, ...) rt_kprintf("[LOGD]" tag format, ##__VA_ARGS__)
#define LOGV(tag, format, ...) rt_kprintf("[LOGV]" tag format, ##__VA_ARGS__)
#define TAG "GC0328"
typedef enum
{
    AWB_MODE_OFF,
    AWB_MODE_AUTO,
    AWB_MODE_CLOUDY_DAYLIGHT,
    AWB_MODE_DAYLIGHT,
    AWB_MODE_INCANDESCENT,
    AWB_MODE_TUNGSTEN,
    AWB_MODE_FLUORESCENT
} AWB_MODE_TYPE;

typedef enum
{
    MEFFECT_OFF,
    MEFFECT_SEPIA,
    MEFFECT_NEGATIVE,
    MEFFECT_SEPIAGREEN,
    MEFFECT_SEPIABLUE,
    MEFFECT_MONO,
} MEFFECT_MODE_TYPE;

typedef enum
{
    AE_FLICKER_MODE_50HZ,
    AE_FLICKER_MODE_60HZ
} AE_FLICKER_MODE_TYPE;

typedef enum
{
    AE_EV_COMP_n13,
    AE_EV_COMP_n10,
    AE_EV_COMP_n07,
    AE_EV_COMP_n03,
    AE_EV_COMP_00,
    AE_EV_COMP_03,
    AE_EV_COMP_07,
    AE_EV_COMP_10,
    AE_EV_COMP_13
} AE_EV_COMP_TYPE;

typedef enum
{
    GammaLvl_01,
    GammaLvl_02,
    GammaLvl_03,
    GammaLvl_04,
    GammaLvl_05
} GAMMALVL_TYPE;

typedef enum
{
    cam_false,
    cam_true
} cam_bool_type;

typedef enum
{
    NIR_CAM = 0,
    RGB_CAM,
    MAX_CAM
} CAM_TYPE;

struct measure_win
{
	int x1;
	int y1;
	int x2;
	int y2;
};
void gc0308_gamma_select(uint8_t num, GAMMALVL_TYPE glvl);
cam_bool_type gc0308_set_param_wb(uint8_t num, AWB_MODE_TYPE para);
cam_bool_type gc0308_set_param_effect(uint8_t num, MEFFECT_MODE_TYPE para);
cam_bool_type gc0308_set_param_banding(uint8_t num, AE_FLICKER_MODE_TYPE para);
cam_bool_type gc0308_set_param_exposure(uint8_t num, AE_EV_COMP_TYPE para);
void gc0308_set_test_patternmode(uint8_t num, cam_bool_type enable);
void gc0308_write_more_registers(uint8_t num);
void gc0308_read_all_reg_setting(uint8_t num);
void gc0308_set_measure_win(CAM_TYPE type,int x1, int y1, int x2, int y2);

static int __gc0308_init(void);
void open_gc0308(CAM_TYPE type);

void change_gc0308(CAM_TYPE type);
void enable_gc0308_output(CAM_TYPE type);
uint8_t gc0308_get_y_value(CAM_TYPE type);

void dvp_face_set_win(int x1, int y1, int x2, int y2);
void dvp_face_get_win(struct measure_win *win);

#define SENSOR_OUTPUT_W		320
#define SENSOR_OUTPUT_H		240

#define SENSOR_EXP_WIN_W	200
#define SENSOR_EXP_WIN_H	180
#define SENSOR_EXP_WIN_W_OFFSET 	(-30)
#define SENSOR_EXP_WIN_W_OFFSET2	(30)

#define EXP_WIN_RECT_X1 (SENSOR_OUTPUT_W - SENSOR_EXP_WIN_W) / 2 + SENSOR_EXP_WIN_W_OFFSET  //90
#define EXP_WIN_RECT_Y1 (SENSOR_OUTPUT_H - SENSOR_EXP_WIN_H) / 2 //30
#define EXP_WIN_RECT_X2 (EXP_WIN_RECT_X1 + SENSOR_EXP_WIN_W + SENSOR_EXP_WIN_W_OFFSET2) //290
#define EXP_WIN_RECT_Y2 (EXP_WIN_RECT_Y1 + SENSOR_EXP_WIN_H) //210

#define ADD_SENSOR_EXPOSURE_WIN

#define  READ_REG_TEST  (0)
#if (READ_REG_TEST)
void gc0308_reg_read_test_PAGE0(uint8_t num);
void gc0308_reg_read_test_PAGE1(uint8_t num);
#endif
#define GC0308_ADDR                     0X42

void gc0308_read_all_reg_setting(uint8_t num);


static const uint8_t gc0308_config[][2] =
{
#if 0
    {0xFE, 0x80},
    {0xFE, 0x00},

    {0x14, 0x12},
    {0x1A, 0x2A},
    {0x1C, 0x49},
    {0x1D, 0x9A},
    {0x1E, 0x61},
    {0x1F, 0x15},
    // {0x22, 0x46},  //crop window mode enable
    {0x24, 0xA6},
    {0x26, 0x3F},
    {0x2E, 0x00},

    {0xd2, 0x10},    // close AEC
    {0x22, 0x55},    // close AWB
    {0x5a, 0x56},
    {0x5b, 0x40},
    {0x5c, 0x4a},
    {0x22, 0x57},   // Open AWB
    {0x01, 0xfa},
    {0x02, 0x70},
    {0x0f, 0x01},

    {0x03, 0x01},
    {0x04, 0x2c},

    {0xe2, 0x00},   //anti-flicker step [11:8]
    {0xe3, 0x64},    //anti-flicker step [7:0]

    {0xe4, 0x02},    //exp level 0  16.67fps
    {0xe5, 0x58},
    {0xe6, 0x03},    //exp level 1  12.5fps
    {0xe7, 0x20},
    {0xe8, 0x04},    //exp level 2  8.33fps
    {0xe9, 0xb0},
    {0xea, 0x09},    //exp level 3  4.00fps
    {0xeb, 0xc4},
    {0x05, 0x00},
    {0x06, 0x00},
    {0x07, 0x00},
    {0x08, 0x00},
    {0x09, 0x01},
    {0x0a, 0xe8},
    {0x0b, 0x02},
    {0x0c, 0x88},
    {0x0d, 0x02},
    {0x0e, 0x02},
    {0x10, 0x22},
    {0x11, 0xfd},
    {0x12, 0x2a},
    {0x13, 0x00},
    {0x14, 0x10},
    {0x15, 0x0a},
    {0x16, 0x05},
    {0x17, 0x01},
    {0x18, 0x44},
    {0x19, 0x44},

    {0x1a, 0x1e},
    {0x1b, 0x00},
    {0x1c, 0xc1},
    {0x1d, 0x08},
    {0x1e, 0x60},
    {0x1f, 0x14},

    {0xFE, 0x01},
    {0x54, 0x22},
    {0xFE, 0x00},
    {0x25, 0x00},
    {0x1f, 0x14}

#else
    /*****************************************************/
    {0xfe, 0x80},
    {0xFE, 0x01},
    {0x54, 0x22},

    {0xfe, 0x00},    // set page0

    {0xd2, 0x10},    // close AEC
    {0x22, 0x55},    // close AWB

	{ 0x03, 0x01 },
//	{ 0x04, 0x2c },
	{ 0x04, 0xf4 },
	{ 0x5a, 0x56 },
	{ 0x5b, 0x40 },
	{ 0x5c, 0x4a },

    {0x22, 0x57},    // Open AWB

    {0x01, 0xfa},
    {0x02, 0x70},
    {0x0f, 0x01},


    {0xe2, 0x00},    //anti-flicker step [11:8]
    {0xe3, 0x64},    //anti-flicker step [7:0]

    {0xe4, 0x02},    //exp level 1  16.67fps
    {0xe5, 0x58},
    {0xe6, 0x03},    //exp level 2  12.5fps
    {0xe7, 0x20},
    {0xe8, 0x04},    //exp level 3  8.33fps
    {0xe9, 0xb0},
    {0xea, 0x09},    //exp level 4  4.00fps
    {0xeb, 0xc4},

    //{0xec , 0x20},

    //  {0x05 , 0x00},              //默认值, 0x05-0x0c register not set(default value),window output is default to 648*488(640*480)
    //  {0x06 , 0x00},
    //  {0x07 , 0x00},
    //  {0x08 , 0x00},
    //  {0x09 , 0x01},
    //  {0x0a , 0xe8},
    //  {0x0b , 0x02},
    //  {0x0c , 0x88},
    //  {0x0d , 0x02},
    {0x0e, 0x02},
    //  {0x10 , 0x22},
    {0x11, 0xfd},
    {0x12, 0x2a},
    //  {0x13 , 0x00},
    /*{0x14 , 0x10}, */
    //  {0x15 , 0x0a},
    //  {0x16 , 0x05},
    {0x17, 0x01},
    //  {0x18 , 0x44},
    //  {0x19 , 0x44},
    {0x1a, 0x1e},
    //  {0x1b , 0x00},
    {0x1c, 0xc1},
    {0x1d, 0x08},
    {0x1e, 0x60},
    {0x1f, 0x17},


    //  {0x20 , 0xff},
    {0x21, 0xf8},
    {0x22, 0x57},
    {0x24, 0xa6},
    {0x25, 0x0f},  //default enable dvp data output,0x00 is disable @tration

    //output sync_mode
    {0x26, 0x02},    //0x03  20101016 zhj
    //  {0x2f , 0x01},
    {0x30, 0xf7},
    {0x31, 0x50},
    {0x32, 0x00},
    {0x39, 0x04},
    {0x3a, 0x18},
    {0x3b, 0x20},
    {0x3c, 0x00},
    {0x3d, 0x00},
    {0x3e, 0x00},
    {0x3f, 0x00},
    {0x50, 0x10},
    {0x53, 0x82},
    //  {0x54 , 0x80},
    //  {0x55 , 0x80},
    {0x56, 0x82},
    //  {0x8b , 0x40},
    //  {0x8c , 0x40},
    //  {0x8d , 0x40},
    {0x8e, 0x2e},
    {0x8f, 0x2e},
    {0x90, 0x2e},
    {0x91, 0x3c},
    {0x92, 0x50},
    //  {0x5d , 0x12},
    //  {0x5e , 0x1a},
    //  {0x5f , 0x24},
    //  {0x60 , 0x07},
    {0x61, 0x15},
    {0x62, 0x08},
    {0x64, 0x03},
    {0x66, 0xe8},
    {0x67, 0x86},
    {0x68, 0xa2},
    {0x69, 0x18},
    {0x6a, 0x0f},
    {0x6b, 0x00},
    {0x6c, 0x5f},
    {0x6d, 0x8f},
    {0x6e, 0x55},
    {0x6f, 0x38},
    {0x70, 0x15},
    {0x71, 0x33},
    {0x72, 0xdc},
    //  {0x73 , 0x80},
    {0x74, 0x02},
    //  {0x75 , 0x3f},
    {0x76, 0x02},
    {0x77, 0x36},
    //  {0x78 , 0x88},
    {0x79, 0x81},
    {0x7a, 0x81},
    //  {0x7b , 0x22},
    {0x7c, 0xff},
    {0x93, 0x48},
    {0x94, 0x00},
    {0x95, 0x05},
    {0x96, 0xe8},
    {0x97, 0x40},
    {0x98, 0xf0},
    {0xb1, 0x38},
    {0xb2, 0x38},
    {0xbd, 0x38},
    {0xbe, 0x36},
    {0xd0, 0xc9},
    {0xd1, 0x10},
    //{0xd2 , 0x90},
    {0xd3, 0x80},
    //  {0xd5 , 0xf2},
    {0xd6, 0x16},
    {0xdb, 0x92},
    {0xdc, 0xa5},
    {0xdf, 0x23},
    {0xd9, 0x00},
    {0xda, 0x00},
    {0xe0, 0x09},

    {0xed, 0x04},
    {0xee, 0xa0},
    {0xef, 0x40},
    //  {0x80 , 0x03},
    //  {0x9F , 0x10},
    //  {0xA0 , 0x20},
    //  {0xA1 , 0x38},
    //  {0xA2 , 0x4E},
    //  {0xA3 , 0x63},
    //  {0xA4 , 0x76},
    //  {0xA5 , 0x87},
    //  {0xA6 , 0xA2},
    //  {0xA7 , 0xB8},
    //  {0xA8 , 0xCA},
    //  {0xA9 , 0xD8},
    //  {0xAA , 0xE3},
    //  {0xAB , 0xEB},
    //  {0xAC , 0xF0},
    //  {0xAD , 0xF8},
    //  {0xAE , 0xFD},
    //  {0xAF , 0xFF},
    {0xc0, 0x00},
    {0xc1, 0x10},
    {0xc2, 0x1C},
    {0xc3, 0x30},
    {0xc4, 0x43},
    {0xc5, 0x54},
    {0xc6, 0x65},
    {0xc7, 0x75},
    {0xc8, 0x93},
    {0xc9, 0xB0},
    {0xca, 0xCB},
    {0xcb, 0xE6},
    {0xcc, 0xFF},
    {0xf0, 0x02},
    {0xf1, 0x01},
    {0xf2, 0x01},
    {0xf3, 0x30},
    {0xf9, 0x9f},
    {0xfa, 0x78},

    //---------------------------------------------------------------
    {0xfe, 0x01}, // set page1

    //  {0x00 , 0xf5},
    {0x02, 0x1a},
    {0x0a, 0xa0},
    //  {0x0b , 0x60},
    {0x0c, 0x08},
    {0x0e, 0x4c},
    {0x0f, 0x39},
    //  {0x11 , 0x3f},
    {0x12, 0x72},
    {0x13, 0x13},
    /*  {0x14 , 0x42},
        {0x15 , 0x43},
        {0x16 , 0xc2},
        {0x17 , 0xa8},
        {0x18 , 0x18},
        {0x19 , 0x40},
        {0x1a , 0xd0},
        {0x1b , 0xf5},
        */
    /*  {0x70 , 0x40},
        {0x71 , 0x58},
        {0x72 , 0x30},
        {0x73 , 0x48},
        {0x74 , 0x20},
        {0x75 , 0x60},
        {0x77 , 0x20},
        {0x78 , 0x32},
    */
    /*  {0x30 , 0x03},
        {0x31 , 0x40},
        {0x32 , 0xe0},
        {0x33 , 0xe0},
        {0x34 , 0xe0},
        {0x35 , 0xb0},
        {0x36 , 0xc0},
        {0x37 , 0xc0},
        {0x38 , 0x04},
        {0x39 , 0x09},
        {0x3a , 0x12},
        {0x3b , 0x1C},
        {0x3c , 0x28},
        {0x3d , 0x31},
        {0x3e , 0x44},
        {0x3f , 0x57},
        */
    /*  {0x40 , 0x6C},
        {0x41 , 0x81},
        {0x42 , 0x94},
        {0x43 , 0xA7},
        {0x44 , 0xB8},
        {0x45 , 0xD6},
        {0x46 , 0xEE},
        {0x47 , 0x0d},
        */
    {0xfe, 0x00},  // set page0

    //-----------Update the registers 2010/07/06-------------//
    //Registers of Page0
    {0xfe, 0x00},  // set page0
    {0x10, 0x26},
    {0x11, 0x0d},   // fd,modified by mormo 2010/07/06
    {0x1a, 0x2a},   // 1e,modified by mormo 2010/07/06

    {0x1c, 0x49},  // c1,modified by mormo 2010/07/06
    {0x1d, 0x9a},  // 08,modified by mormo 2010/07/06
    {0x1e, 0x61},  // 60,modified by mormo 2010/07/06

    {0x3a, 0x20},

    {0x50, 0x14},   // 10,modified by mormo 2010/07/06
    {0x53, 0x80},
    {0x56, 0x80},

    {0x8b, 0x20},  //LSC
    {0x8c, 0x20},
    {0x8d, 0x20},
    {0x8e, 0x14},
    {0x8f, 0x10},
    {0x90, 0x14},

    {0x94, 0x02},
    {0x95, 0x07},
    {0x96, 0xe0},

    {0xb1, 0x40},  // YCPT
    {0xb2, 0x40},
    {0xb3, 0x40},
    {0xb6, 0xe0},

    {0xd0, 0xcb},  // AECT  c9,modifed by mormo 2010/07/06
    {0xd3, 0x48},  // 80,modified by mormor 2010/07/06

    {0xf2, 0x02},
    {0xf7, 0x12},
    {0xf8, 0x0a},

    //Registers of Page1
    {0xfe, 0x01}, // set page1
    {0x02, 0x20},
    {0x04, 0x10},
    {0x05, 0x08},
    {0x06, 0x20},
    {0x08, 0x0a},

    {0x0e, 0x44},
    {0x0f, 0x32},
    {0x10, 0x41},
    {0x11, 0x37},
    {0x12, 0x22},
    {0x13, 0x19},
    {0x14, 0x44},
    {0x15, 0x44},

    {0x19, 0x50},
    {0x1a, 0xd8},

    {0x32, 0x10},

    {0x35, 0x00},
    {0x36, 0x80},
    {0x37, 0x00},
    //-----------Update the registers end---------//


    {0xfe, 0x00},  // set page0
    {0xd2, 0x90},  // open AEC


    //-----------GAMMA Select(3)---------------//
    /*  {0x9F , 0x10},
        {0xA0 , 0x20},
        {0xA1 , 0x38},
        {0xA2 , 0x4E},
        {0xA3 , 0x63},
        {0xA4 , 0x76},
        {0xA5 , 0x87},
        {0xA6 , 0xA2},
        {0xA7 , 0xB8},
        {0xA8 , 0xCA},
        {0xA9 , 0xD8},
        {0xAA , 0xE3},
        {0xAB , 0xEB},
        {0xAC , 0xF0},
        {0xAD , 0xF8},
        {0xAE , 0xFD},
        {0xAF , 0xFF},
    */
    /*GC0308_GAMMA_Select,
    1:                                             //smallest gamma curve
       {0x9F , 0x0B},
       {0xA0 , 0x16},
       {0xA1 , 0x29},
       {0xA2 , 0x3C},
       {0xA3 , 0x4F},
       {0xA4 , 0x5F},
       {0xA5 , 0x6F},
       {0xA6 , 0x8A},
       {0xA7 , 0x9F},
       {0xA8 , 0xB4},
       {0xA9 , 0xC6},
       {0xAA , 0xD3},
       {0xAB , 0xDD},
       {0xAC , 0xE5},
       {0xAD , 0xF1},
       {0xAE , 0xFA},
       {0xAF , 0xFF},

    2:
       {0x9F , 0x0E},
       {0xA0 , 0x1C},
       {0xA1 , 0x34},
       {0xA2 , 0x48},
       {0xA3 , 0x5A},
       {0xA4 , 0x6B},
       {0xA5 , 0x7B},
       {0xA6 , 0x95},
       {0xA7 , 0xAB},
       {0xA8 , 0xBF},
       {0xA9 , 0xCE},
       {0xAA , 0xD9},
       {0xAB , 0xE4},
       {0xAC , 0xEC},
       {0xAD , 0xF7},
       {0xAE , 0xFD},
       {0xAF , 0xFF},

    3:
       {0x9F , 0x10},
       {0xA0 , 0x20},
       {0xA1 , 0x38},
       {0xA2 , 0x4E},
       {0xA3 , 0x63},
       {0xA4 , 0x76},
       {0xA5 , 0x87},
       {0xA6 , 0xA2},
       {0xA7 , 0xB8},
       {0xA8 , 0xCA},
       {0xA9 , 0xD8},
       {0xAA , 0xE3},
       {0xAB , 0xEB},
       {0xAC , 0xF0},
       {0xAD , 0xF8},
       {0xAE , 0xFD},
       {0xAF , 0xFF},

    4:
       {0x9F , 0x14},
       {0xA0 , 0x28},
       {0xA1 , 0x44},
       {0xA2 , 0x5D},
       {0xA3 , 0x72},
       {0xA4 , 0x86},
       {0xA5 , 0x95},
       {0xA6 , 0xB1},
       {0xA7 , 0xC6},
       {0xA8 , 0xD5},
       {0xA9 , 0xE1},
       {0xAA , 0xEA},
       {0xAB , 0xF1},
       {0xAC , 0xF5},
       {0xAD , 0xFB},
       {0xAE , 0xFE},
       {0xAF , 0xFF},

    5:                             //largest gamma curve
       {0x9F , 0x15},
       {0xA0 , 0x2A},
       {0xA1 , 0x4A},
       {0xA2 , 0x67},
       {0xA3 , 0x79},
       {0xA4 , 0x8C},
       {0xA5 , 0x9A},
       {0xA6 , 0xB3},
       {0xA7 , 0xC5},
       {0xA8 , 0xD5},
       {0xA9 , 0xDF},
       {0xAA , 0xE8},
       {0xAB , 0xEE},
       {0xAC , 0xF3},
       {0xAD , 0xFA},
       {0xAE , 0xFD},
       {0xAF , 0xFF}, */
    //-----------GAMMA Select End--------------//




    //-------------H_V_Switch(4)---------------//
    {0x14, 0x12},   //0x10

    /*GC0308_H_V_Switch,

    1:  // normal
               {0x14 , 0x10},

    2:  // IMAGE_H_MIRROR
               {0x14 , 0x11},

    3:  // IMAGE_V_MIRROR
               {0x14 , 0x12},

    4:  // IMAGE_HV_MIRROR
               {0x14 , 0x13},
    */
    //-------------H_V_Select End--------------//
    //add by traion for test
    //{0xec,0x00}, //fixed fps(30fps) @tration

#endif
};

void i2c_master_init(uint8_t num)
{
    if (num)                                            //1,GC0380  RGB
    {
        fpioa_set_function(BSP_CAMERA_SCCB_SDA_PIN, FUNC_I2C1_SDA);
        fpioa_set_function(BSP_CAMERA_SCCB_SCLK_PIN, FUNC_I2C1_SCLK);
    }
    #if 0
    else
    {
        fpioa_set_function(DVP_PIN_SDA0, FUNC_I2C0_SDA);       //0,GC0328  NIR
        fpioa_set_function(DVP_PIN_SCL, FUNC_I2C0_SCLK);
    }
    #endif
    i2c_init(num, GC0308_ADDR >> 1, 7, 100000);

}

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

#define GC0308_SET_PAGE0(n)     gc0308_wr_reg(n,0xfe,0x00)
#define GC0308_SET_PAGE1(n)     gc0308_wr_reg(n,0xfe,0x01)

#if 1
void open_gc0308(CAM_TYPE type)
{
    #if 0
	if (type == NIR_CAM)
	{
		i2c_master_init(1);
		GC0308_SET_PAGE0(1);
		gc0308_wr_reg(1, 0x25, 0x00); //disable visible light data output
		i2c_master_init(0);
		GC0308_SET_PAGE0(0);
		gc0308_wr_reg(0, 0x25, 0x0F); //enable ir data output
	}
	else if (type == RGB_CAM)
	{
		i2c_master_init(0);
		GC0308_SET_PAGE0(0);
		gc0308_wr_reg(0, 0x25, 0x00); //disable ir data output
		i2c_master_init(1);
		GC0308_SET_PAGE0(1);
		gc0308_wr_reg(1, 0x25, 0x0F); //enable visible light data output
	}
    #else
    i2c_master_init(1);
    GC0308_SET_PAGE0(1);
    gc0308_wr_reg(1, 0x25, 0x0F); //enable visible light data output
    #endif
}
#else
void open_gc0308_0(void)
{
    usleep(1 * 1000);
    i2c_master_init(1);
    usleep(1 * 1000);
    GC0308_SET_PAGE0(1);
    gc0308_wr_reg(1, 0x25, 0x00);//disable visible light data output
    usleep(1 * 1000);
    i2c_master_init(0);
    usleep(1 * 1000);
    GC0308_SET_PAGE0(0);
    gc0308_wr_reg(0, 0x25, 0x0F);//enable ir data output
}

void open_gc0308_1(void)
{
    usleep(1 * 1000);
    i2c_master_init(0);
    usleep(1 * 1000);
    GC0308_SET_PAGE0(0);
    gc0308_wr_reg(0, 0x25, 0x00); //disable ir data output
    usleep(1 * 1000);
    i2c_master_init(1);
    usleep(1 * 1000);
    GC0308_SET_PAGE0(1);
    gc0308_wr_reg(1, 0x25, 0x0F); //enable visible light data output
}
#endif

void change_gc0308(CAM_TYPE type)
{
	if (type == NIR_CAM)
	{
		i2c_master_init(1);
		GC0308_SET_PAGE0(1);
		gc0308_wr_reg(1, 0x25, 0x00); //disable visible light data output
		i2c_master_init(0);
		GC0308_SET_PAGE0(0);
//		gc0308_wr_reg(0, 0x25, 0x0F); //enable ir data output

	}
	else if (type == RGB_CAM)
	{
		i2c_master_init(0);
		GC0308_SET_PAGE0(0);
		gc0308_wr_reg(0, 0x25, 0x00); //disable ir data output
		i2c_master_init(1);
		GC0308_SET_PAGE0(1);
//		gc0308_wr_reg(1, 0x25, 0x0F); //enable visible light data output
	}
}

void enable_gc0308_output(CAM_TYPE type)
{
	if (type == NIR_CAM)
	{
		gc0308_wr_reg(0, 0x25, 0x0F); //enable ir data output

	}
	else if (type == RGB_CAM)
	{
		gc0308_wr_reg(1, 0x25, 0x0F); //enable visible light data output
	}
}

int gc0308_read_id(uint8_t num, uint8_t* id)
{
    *id = gc0308_rd_reg(num, 0x00);
    return 0;
}

static void gc0308_AWB_enable(uint8_t num, uint8_t AWB_enable)
{
    uint16_t temp_AWB_reg = 0;
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    temp_AWB_reg = gc0308_rd_reg(num, 0x22);

    if (AWB_enable == cam_true)
    {
        gc0308_wr_reg(num, 0x22, (temp_AWB_reg | 0x02));
    }
    else
    {
        gc0308_wr_reg(num, 0x22, (temp_AWB_reg & (~0x02)));
    }
}

static void  gc0308_set_AE_mode(uint8_t num, cam_bool_type AE_enable)
{
    uint8_t temp_AE_reg = 0;
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    temp_AE_reg = gc0308_rd_reg(num, 0xd2);

    if (AE_enable == cam_true)
    {

        gc0308_wr_reg(num, 0xd2, (temp_AE_reg | 0x80));
    }
    else
    {
        gc0308_wr_reg(num, 0xd2, (temp_AE_reg & (~0x80)));
    }

}


/*************************************************************************
 * FUNCTION
 *  GC0308_NightMode
 *
 * DESCRIPTION
 *  This function night mode of GC0308.
 *
 * PARAMETERS
 *  bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
 *
 * RETURNS
 *  None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
/*void gc0308_night_mode(bool enable)
{
    if (enable)
    {

        if(GC0308_MPEG4_encode_mode == KAL_TRUE)
            GC0308_write_cmos_sensor(0xec, 0x00);
        else
            GC0308_write_cmos_sensor(0xec, 0x30);
        GC0308_NIGHT_MODE = KAL_TRUE;
    }
    else
    {
        if(GC0308_MPEG4_encode_mode == KAL_TRUE)
            GC0308_write_cmos_sensor(0xec, 0x00);
        else
            GC0308_write_cmos_sensor(0xec, 0x20);
        GC0308_NIGHT_MODE = KAL_FALSE;
    }
}
*/

/*************************************************************************
* FUNCTION
*   gc0308_gamma_select
*
* DESCRIPTION
*   This function is served for FAE to select the appropriate GAMMA curve.
*
* PARAMETERS
*   num
*   glvl
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*
*************************************************************************/
void gc0308_gamma_select(uint8_t num, GAMMALVL_TYPE glvl)
{
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    switch (glvl)
    {
        case GammaLvl_01:                            //smallest gamma curve
            gc0308_wr_reg(num, 0x9F, 0x0B);
            gc0308_wr_reg(num, 0xA0, 0x16);
            gc0308_wr_reg(num, 0xA1, 0x29);
            gc0308_wr_reg(num, 0xA2, 0x3C);
            gc0308_wr_reg(num, 0xA3, 0x4F);
            gc0308_wr_reg(num, 0xA4, 0x5F);
            gc0308_wr_reg(num, 0xA5, 0x6F);
            gc0308_wr_reg(num, 0xA6, 0x8A);
            gc0308_wr_reg(num, 0xA7, 0x9F);
            gc0308_wr_reg(num, 0xA8, 0xB4);
            gc0308_wr_reg(num, 0xA9, 0xC6);
            gc0308_wr_reg(num, 0xAA, 0xD3);
            gc0308_wr_reg(num, 0xAB, 0xDD);
            gc0308_wr_reg(num, 0xAC, 0xE5);
            gc0308_wr_reg(num, 0xAD, 0xF1);
            gc0308_wr_reg(num, 0xAE, 0xFA);
            gc0308_wr_reg(num, 0xAF, 0xFF);
            break;

        case GammaLvl_02:
            gc0308_wr_reg(num, 0x9F, 0x0E);
            gc0308_wr_reg(num, 0xA0, 0x1C);
            gc0308_wr_reg(num, 0xA1, 0x34);
            gc0308_wr_reg(num, 0xA2, 0x48);
            gc0308_wr_reg(num, 0xA3, 0x5A);
            gc0308_wr_reg(num, 0xA4, 0x6B);
            gc0308_wr_reg(num, 0xA5, 0x7B);
            gc0308_wr_reg(num, 0xA6, 0x95);
            gc0308_wr_reg(num, 0xA7, 0xAB);
            gc0308_wr_reg(num, 0xA8, 0xBF);
            gc0308_wr_reg(num, 0xA9, 0xCE);
            gc0308_wr_reg(num, 0xAA, 0xD9);
            gc0308_wr_reg(num, 0xAB, 0xE4);
            gc0308_wr_reg(num, 0xAC, 0xEC);
            gc0308_wr_reg(num, 0xAD, 0xF7);
            gc0308_wr_reg(num, 0xAE, 0xFD);
            gc0308_wr_reg(num, 0xAF, 0xFF);
            break;

        case GammaLvl_03:
            gc0308_wr_reg(num, 0x9F, 0x10);
            gc0308_wr_reg(num, 0xA0, 0x20);
            gc0308_wr_reg(num, 0xA1, 0x38);
            gc0308_wr_reg(num, 0xA2, 0x4E);
            gc0308_wr_reg(num, 0xA3, 0x63);
            gc0308_wr_reg(num, 0xA4, 0x76);
            gc0308_wr_reg(num, 0xA5, 0x87);
            gc0308_wr_reg(num, 0xA6, 0xA2);
            gc0308_wr_reg(num, 0xA7, 0xB8);
            gc0308_wr_reg(num, 0xA8, 0xCA);
            gc0308_wr_reg(num, 0xA9, 0xD8);
            gc0308_wr_reg(num, 0xAA, 0xE3);
            gc0308_wr_reg(num, 0xAB, 0xEB);
            gc0308_wr_reg(num, 0xAC, 0xF0);
            gc0308_wr_reg(num, 0xAD, 0xF8);
            gc0308_wr_reg(num, 0xAE, 0xFD);
            gc0308_wr_reg(num, 0xAF, 0xFF);
            break;

        case GammaLvl_04:
            gc0308_wr_reg(num, 0x9F, 0x14);
            gc0308_wr_reg(num, 0xA0, 0x28);
            gc0308_wr_reg(num, 0xA1, 0x44);
            gc0308_wr_reg(num, 0xA2, 0x5D);
            gc0308_wr_reg(num, 0xA3, 0x72);
            gc0308_wr_reg(num, 0xA4, 0x86);
            gc0308_wr_reg(num, 0xA5, 0x95);
            gc0308_wr_reg(num, 0xA6, 0xB1);
            gc0308_wr_reg(num, 0xA7, 0xC6);
            gc0308_wr_reg(num, 0xA8, 0xD5);
            gc0308_wr_reg(num, 0xA9, 0xE1);
            gc0308_wr_reg(num, 0xAA, 0xEA);
            gc0308_wr_reg(num, 0xAB, 0xF1);
            gc0308_wr_reg(num, 0xAC, 0xF5);
            gc0308_wr_reg(num, 0xAD, 0xFB);
            gc0308_wr_reg(num, 0xAE, 0xFE);
            gc0308_wr_reg(num, 0xAF, 0xFF);
            break;

        case GammaLvl_05:                               // largest gamma curve
            gc0308_wr_reg(num, 0x9F, 0x15);
            gc0308_wr_reg(num, 0xA0, 0x2A);
            gc0308_wr_reg(num, 0xA1, 0x4A);
            gc0308_wr_reg(num, 0xA2, 0x67);
            gc0308_wr_reg(num, 0xA3, 0x79);
            gc0308_wr_reg(num, 0xA4, 0x8C);
            gc0308_wr_reg(num, 0xA5, 0x9A);
            gc0308_wr_reg(num, 0xA6, 0xB3);
            gc0308_wr_reg(num, 0xA7, 0xC5);
            gc0308_wr_reg(num, 0xA8, 0xD5);
            gc0308_wr_reg(num, 0xA9, 0xDF);
            gc0308_wr_reg(num, 0xAA, 0xE8);
            gc0308_wr_reg(num, 0xAB, 0xEE);
            gc0308_wr_reg(num, 0xAC, 0xF3);
            gc0308_wr_reg(num, 0xAD, 0xFA);
            gc0308_wr_reg(num, 0xAE, 0xFD);
            gc0308_wr_reg(num, 0xAF, 0xFF);
            break;

        default:
            break;
    }
}

///
/// \brief 设置白平衡
/// \param num
/// \param para
/// \return
///
cam_bool_type gc0308_set_param_wb(uint8_t num, AWB_MODE_TYPE para)
{
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    switch (para)
    {
        case AWB_MODE_OFF:
            gc0308_AWB_enable(num, false);
            break;

        case AWB_MODE_AUTO://for AWB can adjust back
            gc0308_wr_reg(num, 0x5a, 0x56);
            gc0308_wr_reg(num, 0x5b, 0x40);
            gc0308_wr_reg(num, 0x5c, 0x4a);
            gc0308_AWB_enable(num, true);
            break;

        case AWB_MODE_CLOUDY_DAYLIGHT://cloudy
            gc0308_AWB_enable(num, false);
            gc0308_wr_reg(num, 0x5a, 0x8c); //WB_manual_gain
            gc0308_wr_reg(num, 0x5b, 0x50);
            gc0308_wr_reg(num, 0x5c, 0x40);
            break;

        case AWB_MODE_DAYLIGHT://sunny
            gc0308_AWB_enable(num, false);
            gc0308_wr_reg(num, 0x5a, 0x74);
            gc0308_wr_reg(num, 0x5b, 0x52);
            gc0308_wr_reg(num, 0x5c, 0x40);
            break;

        case AWB_MODE_INCANDESCENT://office
            gc0308_AWB_enable(num, false);
            gc0308_wr_reg(num, 0x5a, 0x48);
            gc0308_wr_reg(num, 0x5b, 0x40);
            gc0308_wr_reg(num, 0x5c, 0x5c);
            break;

        case AWB_MODE_TUNGSTEN://home
            gc0308_AWB_enable(num, false);
            gc0308_wr_reg(num, 0x5a, 0x40);
            gc0308_wr_reg(num, 0x5b, 0x54);
            gc0308_wr_reg(num, 0x5c, 0x70);
            break;

        case AWB_MODE_FLUORESCENT:
            gc0308_AWB_enable(num, false);
            gc0308_wr_reg(num, 0x5a, 0x40);
            gc0308_wr_reg(num, 0x5b, 0x54);
            gc0308_wr_reg(num, 0x5c, 0x70);
            break;

        default:
            return cam_false;
    }

    return cam_true;

}/* GC0308_set_param_wb */

cam_bool_type gc0308_set_param_effect(uint8_t num, MEFFECT_MODE_TYPE para)
{
    bool  ret = cam_true;
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    switch (para)
    {
        case MEFFECT_OFF:
            gc0308_wr_reg(num, 0x23, 0x00);
            gc0308_wr_reg(num, 0x2d, 0x0a);
            gc0308_wr_reg(num, 0x20, 0xff);
            gc0308_wr_reg(num, 0xd2, 0x90);
            gc0308_wr_reg(num, 0x73, 0x00);
            gc0308_wr_reg(num, 0x77, 0x54);

            gc0308_wr_reg(num, 0xb3, 0x40);
            gc0308_wr_reg(num, 0xb4, 0x80);
            gc0308_wr_reg(num, 0xba, 0x00);
            gc0308_wr_reg(num, 0xbb, 0x00);
            break;

        case MEFFECT_SEPIA:
            gc0308_wr_reg(num, 0x23, 0x02);
            gc0308_wr_reg(num, 0x2d, 0x0a); // 0x08
            gc0308_wr_reg(num, 0x20, 0xff);
            gc0308_wr_reg(num, 0xd2, 0x90);
            gc0308_wr_reg(num, 0x73, 0x00);

            gc0308_wr_reg(num, 0xb3, 0x40);
            gc0308_wr_reg(num, 0xb4, 0x80);
            gc0308_wr_reg(num, 0xba, 0xd0);
            gc0308_wr_reg(num, 0xbb, 0x28);
            break;

        case MEFFECT_NEGATIVE:
            gc0308_wr_reg(num, 0x23, 0x01);
            gc0308_wr_reg(num, 0x2d, 0x0a);
            gc0308_wr_reg(num, 0x20, 0xff);
            gc0308_wr_reg(num, 0xd2, 0x90);
            gc0308_wr_reg(num, 0x73, 0x00);

            gc0308_wr_reg(num, 0xb3, 0x40);
            gc0308_wr_reg(num, 0xb4, 0x80);
            gc0308_wr_reg(num, 0xba, 0x00);
            gc0308_wr_reg(num, 0xbb, 0x00);
            break;

        case MEFFECT_SEPIAGREEN:
            gc0308_wr_reg(num, 0x23, 0x02);
            gc0308_wr_reg(num, 0x2d, 0x0a);
            gc0308_wr_reg(num, 0x20, 0xff);
            gc0308_wr_reg(num, 0xd2, 0x90);
            gc0308_wr_reg(num, 0x77, 0x88);

            gc0308_wr_reg(num, 0xb3, 0x40);
            gc0308_wr_reg(num, 0xb4, 0x80);
            gc0308_wr_reg(num, 0xba, 0xc0);
            gc0308_wr_reg(num, 0xbb, 0xc0);
            break;

        case MEFFECT_SEPIABLUE:
            gc0308_wr_reg(num, 0x23, 0x02);
            gc0308_wr_reg(num, 0x2d, 0x0a);
            gc0308_wr_reg(num, 0x20, 0xff);
            gc0308_wr_reg(num, 0xd2, 0x90);
            gc0308_wr_reg(num, 0x73, 0x00);

            gc0308_wr_reg(num, 0xb3, 0x40);
            gc0308_wr_reg(num, 0xb4, 0x80);
            gc0308_wr_reg(num, 0xba, 0x50);
            gc0308_wr_reg(num, 0xbb, 0xe0);
            break;

        case MEFFECT_MONO:
            gc0308_wr_reg(num, 0x23, 0x02);
            gc0308_wr_reg(num, 0x2d, 0x0a);
            gc0308_wr_reg(num, 0x20, 0xff);
            gc0308_wr_reg(num, 0xd2, 0x90);
            gc0308_wr_reg(num, 0x73, 0x00);

            gc0308_wr_reg(num, 0xb3, 0x40);
            gc0308_wr_reg(num, 0xb4, 0x80);
            gc0308_wr_reg(num, 0xba, 0x00);
            gc0308_wr_reg(num, 0xbb, 0x00);
            break;

        default:
            ret = cam_false;
            break;
    }

    return ret;

}/* GC0308_set_param_effect */


cam_bool_type gc0308_set_param_banding(uint8_t num, AE_FLICKER_MODE_TYPE para)
{
    bool  ret = cam_true;
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    switch (para)
    {
        case AE_FLICKER_MODE_50HZ:
            gc0308_wr_reg(num, 0x01, 0x26);
            gc0308_wr_reg(num, 0x02, 0x98);
            gc0308_wr_reg(num, 0x0f, 0x03);

            gc0308_wr_reg(num, 0xe2, 0x00);     //anti-flicker step [11:8]
            gc0308_wr_reg(num, 0xe3, 0x50);     //anti-flicker step [7:0]

            gc0308_wr_reg(num, 0xe4, 0x02);     //exp level 0  12.5fps
            gc0308_wr_reg(num, 0xe5, 0x80);
            gc0308_wr_reg(num, 0xe6, 0x03);     //exp level 1  10fps
            gc0308_wr_reg(num, 0xe7, 0x20);
            gc0308_wr_reg(num, 0xe8, 0x04);     //exp level 2  7.69fps
            gc0308_wr_reg(num, 0xe9, 0x10);
            gc0308_wr_reg(num, 0xea, 0x06);     //exp level 3  5.00fps
            gc0308_wr_reg(num, 0xeb, 0x40);
            break;

        case AE_FLICKER_MODE_60HZ:
            gc0308_wr_reg(num, 0x01, 0x97);
            gc0308_wr_reg(num, 0x02, 0x84);
            gc0308_wr_reg(num, 0x0f, 0x03);

            gc0308_wr_reg(num, 0xe2, 0x00);     //anti-flicker step [11:8]
            gc0308_wr_reg(num, 0xe3, 0x3e);   //anti-flicker step [7:0]

            gc0308_wr_reg(num, 0xe4, 0x02);   //exp level 0  12.00fps
            gc0308_wr_reg(num, 0xe5, 0x6c);
            gc0308_wr_reg(num, 0xe6, 0x02);   //exp level 1  10.00fps
            gc0308_wr_reg(num, 0xe7, 0xe8);
            gc0308_wr_reg(num, 0xe8, 0x03);   //exp level 2  7.50fps
            gc0308_wr_reg(num, 0xe9, 0xe0);
            gc0308_wr_reg(num, 0xea, 0x05);   //exp level 3  5.00fps
            gc0308_wr_reg(num, 0xeb, 0xd0);
            break;

        default:
            ret = cam_false;
            break;
    }

    return ret;
}/* GC0308_set_param_banding */

///
/// \brief 设置曝光区域
/// \param num
/// \param para
/// \return
///
cam_bool_type gc0308_set_param_exposure(uint8_t num, AE_EV_COMP_TYPE para)
{
	bool  ret = cam_true;
//	usleep(1 * 1000);
//	i2c_master_init(num);
//	usleep(1 * 1000);

	gc0308_wr_reg(num, 0xd2, 0x00);
	switch (para)
	{
	case AE_EV_COMP_n13:
		gc0308_wr_reg(num, 0xb5, 0xc0);
		gc0308_wr_reg(0, 0xfb, 0x02);
//		gc0308_wr_reg(num, 0xd3, 0x30);
		gc0308_wr_reg(num, 0xd3, 0x20);
		break;

	case AE_EV_COMP_n10:
		gc0308_wr_reg(num, 0xb5, 0xd0);
		gc0308_wr_reg(0, 0xfb, 0x04);
//		gc0308_wr_reg(num, 0xd3, 0x38);
		gc0308_wr_reg(num, 0xd3, 0x25);
		break;

	case AE_EV_COMP_n07:
		gc0308_wr_reg(num, 0xb5, 0xe0);
		gc0308_wr_reg(0, 0xfb, 0x02);
//		gc0308_wr_reg(num, 0xd3, 0x40);
		gc0308_wr_reg(num, 0xd3, 0x28);
		break;

	case AE_EV_COMP_n03:
		gc0308_wr_reg(num, 0xb5, 0xf0);
//		gc0308_wr_reg(num, 0xd3, 0x48);
		gc0308_wr_reg(num, 0xd3, 0x30);
		break;

	case AE_EV_COMP_00:
		gc0308_wr_reg(num, 0xb5, 0x00);
//		gc0308_wr_reg(num, 0xd3, 0x50);
		gc0308_wr_reg(num, 0xd3, 0x38);
		break;

	case AE_EV_COMP_03:
		gc0308_wr_reg(num, 0xb5, 0x10);
		gc0308_wr_reg(num, 0xd3, 0x60);
		break;

	case AE_EV_COMP_07:
		gc0308_wr_reg(num, 0xb5, 0x20);
		gc0308_wr_reg(num, 0xd3, 0x70);
		break;

	case AE_EV_COMP_10:
		gc0308_wr_reg(num, 0xb5, 0x30);
		gc0308_wr_reg(num, 0xd3, 0x80);
		break;

	case AE_EV_COMP_13:
		gc0308_wr_reg(num, 0xb5, 0x40);
		gc0308_wr_reg(num, 0xd3, 0x90);
		break;
	default:
		ret = cam_false;
	}
	gc0308_wr_reg(num, 0xd2, 0xc0);
	return ret;
} /* GC0308_set_param_exposure */


void gc0308_set_test_patternmode(uint8_t num, cam_bool_type enable)
{
    LOGI(TAG, "test carama num:=%d", num);
    LOGI(TAG, "test pattern enable:=%d", enable);

    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    if (enable)
    {
        gc0308_wr_reg(num, 0xfe, 0x00);
        gc0308_wr_reg(num, 0xfa, 0x32);
        gc0308_wr_reg(num, 0x20, 0x08);
        gc0308_wr_reg(num, 0x21, 0x00);
        gc0308_wr_reg(num, 0x22, 0x00);
        gc0308_wr_reg(num, 0xd2, 0x10);
        gc0308_wr_reg(num, 0x15, 0x06);
        gc0308_wr_reg(num, 0xf4, 0xc0);
        gc0308_wr_reg(num, 0xf5, 0x40);
        gc0308_wr_reg(num, 0x5a, 0x40);
        gc0308_wr_reg(num, 0x5b, 0x40);
        gc0308_wr_reg(num, 0x5c, 0x40);
        gc0308_wr_reg(num, 0xb0, 0x40);
        gc0308_wr_reg(num, 0xbd, 0x00);
        gc0308_wr_reg(num, 0x51, 0x40);
        gc0308_wr_reg(num, 0x52, 0x40);
        gc0308_wr_reg(num, 0x03, 0x00);
        gc0308_wr_reg(num, 0x04, 0x00);
        gc0308_wr_reg(num, 0x2e, 0x01);
    }
    else
    {
        gc0308_wr_reg(num, 0xfe, 0x00);
        gc0308_wr_reg(num, 0x2e, 0x00); //test pattern
    }
}

/*************************************************************************
* FUNCTION
*   GC0308_Write_More_Registers
*
* DESCRIPTION
*   This function is served for FAE to modify the necessary Init Regs. Do not modify the regs
*     in init_GC0308() directly.
*
* PARAMETERS
*   num
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void gc0308_write_more_registers(uint8_t num)
{
    //  TODO: FAE Modify the Init Regs here!!!

    //-----------Update the registers 2010/07/07-------------//

    //Registers of Page0
    usleep(1 * 1000);
    i2c_master_init(num);
    usleep(1 * 1000);

    GC0308_SET_PAGE0(num);

    gc0308_wr_reg(num, 0x10, 0x26);
    gc0308_wr_reg(num, 0x11, 0x0d); // fd,modified by mormo 2010/07/06
    gc0308_wr_reg(num, 0x1a, 0x2a); // 1e,modified by mormo 2010/07/06

    gc0308_wr_reg(num, 0x1c, 0x49); // c1,modified by mormo 2010/07/06
    gc0308_wr_reg(num, 0x1d, 0x9a); // 08,modified by mormo 2010/07/06
    gc0308_wr_reg(num, 0x1e, 0x61); // 60,modified by mormo 2010/07/06

    gc0308_wr_reg(num, 0x3a, 0x20);

    gc0308_wr_reg(num, 0x50, 0x14); // 10,modified by mormo 2010/07/06
    gc0308_wr_reg(num, 0x53, 0x80);
    gc0308_wr_reg(num, 0x56, 0x80);

    gc0308_wr_reg(num, 0x8b, 0x20); //LSC
    gc0308_wr_reg(num, 0x8c, 0x20);
    gc0308_wr_reg(num, 0x8d, 0x20);
    gc0308_wr_reg(num, 0x8e, 0x14);
    gc0308_wr_reg(num, 0x8f, 0x10);
    gc0308_wr_reg(num, 0x90, 0x14);

    gc0308_wr_reg(num, 0x94, 0x02);
    gc0308_wr_reg(num, 0x95, 0x07);
    gc0308_wr_reg(num, 0x96, 0xe0);

    gc0308_wr_reg(num, 0xb1, 0x40); // YCPT
    gc0308_wr_reg(num, 0xb2, 0x40);
    gc0308_wr_reg(num, 0xb3, 0x40);
    gc0308_wr_reg(num, 0xb6, 0xe0);

    gc0308_wr_reg(num, 0xd0, 0xcb); // AECT  c9,modifed by mormo 2010/07/06
    gc0308_wr_reg(num, 0xd3, 0x48); // 80,modified by mormor 2010/07/06

    gc0308_wr_reg(num, 0xf2, 0x02);
    gc0308_wr_reg(num, 0xf7, 0x12);
    gc0308_wr_reg(num, 0xf8, 0x0a);

    //Registers of Page1
    GC0308_SET_PAGE1(num);

    gc0308_wr_reg(num, 0x02, 0x20);
    gc0308_wr_reg(num, 0x04, 0x10);
    gc0308_wr_reg(num, 0x05, 0x08);
    gc0308_wr_reg(num, 0x06, 0x20);
    gc0308_wr_reg(num, 0x08, 0x0a);

    gc0308_wr_reg(num, 0x0e, 0x44);
    gc0308_wr_reg(num, 0x0f, 0x32);
    gc0308_wr_reg(num, 0x10, 0x41);
    gc0308_wr_reg(num, 0x11, 0x37);
    gc0308_wr_reg(num, 0x12, 0x22);
    gc0308_wr_reg(num, 0x13, 0x19);
    gc0308_wr_reg(num, 0x14, 0x44);
    gc0308_wr_reg(num, 0x15, 0x44);

    gc0308_wr_reg(num, 0x19, 0x50);
    gc0308_wr_reg(num, 0x1a, 0xd8);

    gc0308_wr_reg(num, 0x32, 0x10);

    gc0308_wr_reg(num, 0x35, 0x00);
    gc0308_wr_reg(num, 0x36, 0x80);
    gc0308_wr_reg(num, 0x37, 0x00);
    //-----------Update the registers end---------//

    GC0308_SET_PAGE0(num);
    /*Customer can adjust GAMMA, MIRROR & UPSIDEDOWN here!*/

    gc0308_gamma_select(num, 2);

}

uint8_t get_luma_average(CAM_TYPE _cam_type)
{
    uint8_t luma_value = 0;

    GC0308_SET_PAGE0(_cam_type);
    luma_value = gc0308_rd_reg(_cam_type, 0xd4);
    GC0308_SET_PAGE0(_cam_type);

    debug_log("_cam_type:%s, luma_value:%d", (_cam_type == NIR_CAM) ? "NIR_CAM" : "RGB_CAM", luma_value);

    return luma_value;
}

void gc0308_set_measure_win(CAM_TYPE type, int x1, int y1, int x2, int y2)
{
	int cam_x1 = x1 * 2 / 4;
	int cam_y1 = y1 * 2 / 4;
	int cam_x2 = x2 * 2 / 4;
	int cam_y2 = y2 * 2 / 4;
//	LOG_I("x1:%d,y1:%d,x2:%d,y2:%d",cam_x1,cam_y1,cam_x2,cam_y2);
	if (cam_x1 == 0 && cam_y1 == 0 && cam_x2 == 0 && cam_y2 == 0)
	{
		cam_x1 = EXP_WIN_RECT_X1 * 2 / 4;
		cam_y1 = EXP_WIN_RECT_Y1 * 2 / 4;
		cam_x2 = EXP_WIN_RECT_X2 * 2 / 4;
		cam_y2 = EXP_WIN_RECT_Y2 * 2 / 4;
	}
	else if (cam_x1 == 0)
	{
		cam_x1 += 4;
	}
	else if (cam_y1 == 0)
	{
		cam_y1 += 4;
	}
	else if (cam_x2 == 0)
	{
		cam_x2 += 4;
	}
	else if (cam_y2 == 0)
	{
		cam_y2 += 4;
	}

	gc0308_wr_reg(type, 0xd2, 0x00);
	gc0308_wr_reg(type, 0xf7, cam_x1);
	gc0308_wr_reg(type, 0xf8, cam_y1);
	gc0308_wr_reg(type, 0xf9, cam_x2);
	gc0308_wr_reg(type, 0xfa, cam_y2);
	gc0308_wr_reg(type, 0xd2, 0xc0);
}

static void gc0308_more_reg_set(CAM_TYPE type)
{
    /*
    ADD_SENSOR_EXPOSURE_WIN在board_config.h中定义
    不开启ADD_SENSOR_EXPOSURE_WIN时默认是全局曝光
    */
#ifdef ADD_SENSOR_EXPOSURE_WIN
    /*
        设置曝光窗口，坐标位置会在屏幕上显示
        1、摄像头输出是抽样320X240的，曝光窗口是640X480来计算的，所以要先X2,
           曝光窗口寄存器参数内部由X4的操作，所以要/4;
        2、寄存器0xf7和0xf8最大为0x3f，所以要做是否超出3f的判断；
    */
    uint16_t x1, y1, x2, y2;

    x1 = EXP_WIN_RECT_X1 * 2 / 4;
    y1 = EXP_WIN_RECT_Y1 * 2 / 4;
    x2 = EXP_WIN_RECT_X2 * 2 / 4;
    y2 = EXP_WIN_RECT_Y2 * 2 / 4;

    if (x1 > 0x3f)
    {
        x1 = 0x3f;    //the x1 max value is 3f
    }

    if (y1 > 0x3f)
    {
        y1 = 0x3f;    //the y1 max value is 3f
    }

#endif

    if (NIR_CAM == type)
    {
        //debug_log("NIR_CAM gc0308_more_reg_set");

        GC0308_SET_PAGE0(0);
#if 0
//		gc0308_wr_reg(0,0xb3, 0x38); //contrast
		gc0308_wr_reg(0,0xd3, 0x28); //target-y value
		gc0308_gamma_select(0,GammaLvl_01); //gamma

#ifdef ADD_SENSOR_EXPOSURE_WIN //AEC window

		gc0308_wr_reg(0,0xd2,0xc0); //开启AEC
//		gc0308_wr_reg(0,0xd2,0x00);	//关闭AEC
		//6灯的增益
//		gc0308_wr_reg(0,0x50,0x0c);	//gain
		//8灯的增益
//		gc0308_wr_reg(0,0x50,0x08);	//gain

		gc0308_wr_reg(0,0xd0,0xca);
		gc0308_wr_reg(0,0xd1,0x10);
		gc0308_wr_reg(0,0xf7,x1); //x0 max 3f=63 63*4 =252
		gc0308_wr_reg(0,0xf8,y1); //y0 max 3f=63 1E=30 30*4=120
		gc0308_wr_reg(0,0xf9,x2); //x1 max A0=160(640/4) 8C=140 140*4=560
		gc0308_wr_reg(0,0xfa,y2); //y1 max 78=120(480/4) 5A=90 90*4=360
#else
		gc0308_wr_reg(0,0xd2,0xc0);
		gc0308_wr_reg(0,0xd0,0xca);
		gc0308_wr_reg(0,0xd1,0x10);
		gc0308_wr_reg(0,0xf7,0x3F); //x0 max 3f=63 63*4 =252
		gc0308_wr_reg(0,0xf8,0x1E); //y0 max 3f=63 1E=30 30*4=120
		gc0308_wr_reg(0,0xf9,0x8C); //x1 max A0=160(640/4) 8C=140 140*4=560
		gc0308_wr_reg(0,0xfa,0x5A); //y1 max 78=120(480/4) 5A=90 90*4=360
#endif
#endif
//		gc0308_wr_reg(0, 0xb3, 0x50);           //contrast
//		gc0308_wr_reg(0, 0xd3, 0x28);           //target-y value


#ifdef ADD_SENSOR_EXPOSURE_WIN //AEC window

		gc0308_wr_reg(0, 0xd2, 0x00);             //关闭AEC
//		gc0308_wr_reg(0, 0xb3, 0x50);           //contrast

//		gc0308_wr_reg(0,0x50,0x3f);				//gain

		gc0308_wr_reg(0, 0xd3, 0x30);           //target-y value
		gc0308_gamma_select(0, GammaLvl_01);    //gamma

		gc0308_wr_reg(0, 0xd0, 0xca);
		gc0308_wr_reg(0, 0xd1, 0x10);
		gc0308_wr_reg(0, 0xf7, x1);             //x0 max 3f=63 63*4 =252
		gc0308_wr_reg(0, 0xf8, y1);             //y0 max 3f=63 1E=30 30*4=120
		gc0308_wr_reg(0, 0xf9, x2);             //x1 max A0=160(640/4) 8C=140 140*4=560
		gc0308_wr_reg(0, 0xfa, y2);             //y1 max 78=120(480/4) 5A=90 90*4=360
		gc0308_wr_reg(0, 0xfb, 0x05);
		gc0308_wr_reg(0, 0xd2, 0xc0);           //开启AEC
#else
		gc0308_wr_reg(0, 0xd2, 0xc0);
		gc0308_wr_reg(0, 0xd0, 0xca);
		gc0308_wr_reg(0, 0xd1, 0x10);
		gc0308_wr_reg(0, 0xf7, 0x3F); //x0 max 3f=63 63*4 =252
		gc0308_wr_reg(0, 0xf8, 0x1E); //y0 max 3f=63 1E=30 30*4=120
		gc0308_wr_reg(0, 0xf9, 0x8C); //x1 max A0=160(640/4) 8C=140 140*4=560
		gc0308_wr_reg(0, 0xfa, 0x5A); //y1 max 78=120(480/4) 5A=90 90*4=360
#endif

#ifdef ENABLE_UYVY_OUTPUT
        /*
        UYVY 0xA0
        VYUY 0xA1
        YUYV 0xA2
        YVYU 0xA3
        RGB565 0xA6
        ONLY Y 0xB1
        */
        gc0308_wr_reg(0, 0x24, 0xA0);
#endif
        //gc0308_set_test_patternmode(0,cam_true);

        GC0308_SET_PAGE0(0);
    }
    else if (RGB_CAM == type)
    {
        //debug_log("RGB_CAM gc0308_more_reg_set");

        GC0308_SET_PAGE0(1);

		gc0308_wr_reg(1, 0xb3, 0x38); //contrast
		gc0308_wr_reg(1, 0xd3, 0x45); //target-y value
		gc0308_gamma_select(1, GammaLvl_05); //gamma GammaLvl_01


#ifdef ADD_SENSOR_EXPOSURE_WIN //AEC window
		gc0308_wr_reg(1, 0xd2, 0x00);             //关闭AEC
		gc0308_wr_reg(1, 0xd2, 0xe0);
		gc0308_wr_reg(1, 0xd0, 0xca);
		gc0308_wr_reg(1, 0xd1, 0x10);
		gc0308_wr_reg(1, 0xf7, x1); //x0 max 3f=63 63*4 =252
		gc0308_wr_reg(1, 0xf8, y1); //y0 max 3f=63 1E=30 30*4=120
		gc0308_wr_reg(1, 0xf9, x2); //x1 max A0=160(640/4) 8C=140 140*4=560
		gc0308_wr_reg(1, 0xfa, y2); //y1 max 78=120(480/4) 5A=90 90*4=360
		gc0308_wr_reg(1, 0xd2, 0xf0);            //开启AEC
#else
		gc0308_wr_reg(1, 0xd2, 0xc0);
		gc0308_wr_reg(1, 0xd0, 0xca);
		gc0308_wr_reg(1, 0xd1, 0x10);
		gc0308_wr_reg(1, 0xf7, 0x3F); //x0 max 3f=63 63*4 =252
		gc0308_wr_reg(1, 0xf8, 0x1E); //y0 max 3f=63 1E=30 30*4=120
		gc0308_wr_reg(1, 0xf9, 0x8C); //x1 max A0=160(640/4) 8C=140 140*4=560
		gc0308_wr_reg(1, 0xfa, 0x5A); //y1 max 78=120(480/4) 5A=90 90*4=360
#endif

#if 0 //AEC window
		gc0308_wr_reg(1,0xd2,0xc0);
		gc0308_wr_reg(1,0xd0,0xca);
		gc0308_wr_reg(1,0xd1,0x10);
		gc0308_wr_reg(1,0xf7,0x6e); //110
		gc0308_wr_reg(1,0xf8,0x6e);
		gc0308_wr_reg(1,0xf9,0x7E); //126
		gc0308_wr_reg(1,0xfa,0x7E);
		//gc0308_set_param_exposure(1, AE_EV_COMP_n10);
#endif

#ifdef ENABLE_UYVY_OUTPUT
		/*
		UYVY 0xA0
		VYUY 0xA1
		YUYV 0xA2
		YVYU 0xA3
		RGB565 0xA6
		ONLY Y 0xB1
		*/
		gc0308_wr_reg(1, 0x24, 0xA0);
#endif
		//gc0308_set_test_patternmode(1,cam_true);

		GC0308_SET_PAGE0(1);
	}
}

static int __gc0308_init(void)
{
    uint8_t data;
   	uint32_t i;
	uint8_t num = 0;
    #if 0
    {

        i2c_master_init(num);
        gc0308_read_id(num, &data);
        //debug_log("gc0308 0 ID : 0x%x", data);
        LOGI(TAG, "gc0308 0 ID : 0x%x", data);

		for (i = 0; gc0308_config[i][0]; i++)
        {
            gc0308_wr_reg(num, gc0308_config[i][0], gc0308_config[i][1]);
            //data = gc0308_rd_reg(num, gc0308_config[i][0]);
            //printf("0x%02x:0x%02x\n", gc0308_config[i][0], data);
        }

        /* set more nir-sensor reg */
        gc0308_more_reg_set(NIR_CAM);
        usleep(1 * 1000);
    }
    #endif
    {
        num = 1;
        i2c_master_init(num);
        gc0308_read_id(num, &data);
        LOGI(TAG, "gc0308 1 ID : 0x%x", data);

		for (i = 0; gc0308_config[i][0]; i++)
        {
            gc0308_wr_reg(num, gc0308_config[i][0], gc0308_config[i][1]);
            //data = gc0308_rd_reg(num, gc0308_config[i][0]);
            //printf("0x%02x:0x%02x\n", gc0308_config[i][0], data);
        }

        /* set more rgb-sensor reg */
        gc0308_more_reg_set(RGB_CAM);
        usleep(1 * 1000);
    }

#if (READ_REG_TEST)
    gc0308_reg_read_test_PAGE0(0);
    gc0308_reg_read_test_PAGE0(1);
#endif

	printf("sensor init ok\r\n");

    return 0;
}


void gc0308_read_all_reg_setting(uint8_t num)
{
    uint8_t data;
    uint32_t i;
    i2c_master_init(num);
    LOGI(TAG, "Carama Num:%d", num);
    GC0308_SET_PAGE0(num);

    for (i = 0; gc0308_config[i][0]; i++)
    {
        if (gc0308_config[i][0] == 0xfe && gc0308_config[i][1] == 0x00)
        {
            GC0308_SET_PAGE0(num);
        }

        if (gc0308_config[i][0] == 0xfe && gc0308_config[i][1] == 0x01)
        {
            GC0308_SET_PAGE1(num);
        }

        data = gc0308_rd_reg(num, gc0308_config[i][0]);
        //LOGI(TAG,"0x%02x:0x%02x %s", gc0308_config[i][0], data, gc0308_config[i][1] == data ? "OK" : "FAILED");
        LOGI(TAG, "0x%02x:0x%02x", gc0308_config[i][0], data);
    }
}

uint8_t gc0308_get_y_value(CAM_TYPE type)
{
    return gc0308_rd_reg(type, 0xd4);
}

#if (READ_REG_TEST)
static const uint8_t gc0308_reg_test[][2] =
{
    //is AEC enable?
    {0xd2, 0xff},//
    //windowing    default to 648*488
    {0x05, 0xff},//0x00
    {0x06, 0xff},//0x00
    {0x07, 0xff},//0x00
    {0x08, 0xff},//0x00
    {0x09, 0xff},//0x01
    {0x0a, 0xff},//0xe8
    {0x0b, 0xff},//0x02
    {0x0c, 0xff},//0x88
    //crop window  default is 0x00(disable crop window mode)
    {0x46, 0xff},//0x00
    //AEC
    {0x53, 0xff},//0x00
    {0x54, 0xff},//0x00
    {0x55, 0xff},//0x00
    {0x56, 0xff},//0x00
    {0x57, 0xff},//0x00
    {0x58, 0xff},//0x00
    {0x59, 0xff},//0x00
    //GAIN
    {0xee, 0xff},//0x00
    {0xef, 0xff},//0x00
    //exp level select
    {0xec, 0xff},
    //Target Y
    {0xd3, 0xff},
    //fixed fps
    {0xec, 0xff},
    //measure window
    {0xf7, 0xff},
    {0xf8, 0xff},
    {0xf9, 0xff},
    {0xfa, 0xff},
    {0xb3, 0xff},
};

static const uint8_t gc0308_reg_test_PAGE1[][2] =
{
    //subsample
    {0x53, 0xff},//0x00
    {0x54, 0xff},//0x00
    {0x55, 0xff},//0x00
    {0x56, 0xff},//0x00
    {0x57, 0xff},//0x00
    {0x58, 0xff},//0x00
    {0x59, 0xff},//0x00
};

void gc0308_reg_read_test_PAGE0(uint8_t num)
{
    uint8_t data;
    i2c_master_init(num);
    LOGD(TAG, "Carama Num:%d", num);

    LOGD(TAG, "*******start read reg from PAGE0**********");
    uint32_t reg_max = sizeof(gc0308_reg_test) / sizeof(gc0308_reg_test[0]);
    LOGD(TAG, "reg_max:%d", reg_max);

    for (int i = 0; i < reg_max; i++)
    {
        data = gc0308_rd_reg(num, gc0308_reg_test[i][0]);
        LOGD(TAG, "{0x%02x,0x%02x}", gc0308_reg_test[i][0], data);
    }

    LOGD(TAG, "*******end read reg from PAGE0**********");
}

void gc0308_reg_read_test_PAGE1(uint8_t num)
{
    uint8_t data;
    i2c_master_init(num);
    LOGD(TAG, "Carama Num:%d", num);

    LOGD(TAG, "*******start read reg from PAGE1**********");
    uint32_t reg_max = sizeof(gc0308_reg_test_PAGE1) / sizeof(gc0308_reg_test_PAGE1[0]);
    LOGD(TAG, "reg_max:%d", reg_max);
    GC0308_SET_PAGE1(num);

    for (int i = 0; i < reg_max; i++)
    {
        data = gc0308_rd_reg(num, gc0308_reg_test_PAGE1[i][0]);
        LOGD(TAG, "{0x%02x,0x%02x}", gc0308_reg_test_PAGE1[i][0], data);
    }

    GC0308_SET_PAGE0(num);
    LOGD(TAG, "*******end read reg from PAGE1**********");
}
#endif

#if 0
static struct rt_device _gc0308;
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



rt_err_t  drv_gc0308_init(rt_device_t dev){
    
    dvp_init(8);  /* DVP初始化，设置sccb的寄存器长度为8bit */
    dvp_set_xclk_rate(24000000);  /* 设置输入时钟为24000000*/
    dvp_enable_burst();  /* 使能突发传输模式 */
    dvp_set_output_enable(0, 1);  /* 关闭AI输出模式，使能显示模式 */
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);  /* 设置输出格式为RGB */
    dvp_set_image_size(320, 240);  /* 设置输出像素大小为320*240 */
    gc0308_init();  /* 摄像头初始化 */
    return RT_EOK;
}

rt_err_t  drv_gc0308_open(rt_device_t dev, rt_uint16_t oflag){
    open_gc0308(RGB_CAM);
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

rt_err_t  drv_gc0308_close(rt_device_t dev){
    
    plic_irq_disable(IRQN_DVP_INTERRUPT);
    return RT_EOK;
}

rt_size_t drv_gc0308_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size){
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

rt_size_t drv_gc0308_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size){
    /* no need */
    return RT_EOK;
}

rt_err_t  drv_gc0308_control(rt_device_t dev, int cmd, void *args){
    /* no need */
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops drv_gc0308_ops =
{
    drv_gc0308_init,
    drv_gc0308_open,
    drv_gc0308_close,
    drv_gc0308_read,
    drv_gc0308_write,
    drv_gc0308_control
};
#endif


int rt_hw_gc0308_init(void)
{
    rt_err_t ret = RT_EOK;

    _gc0308.type        = RT_Device_Class_Sensor;
    _gc0308.rx_indicate = RT_NULL;
    _gc0308.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    _gc0308.ops        = &drv_gc0308_ops;
#else
    _gc0308.init    = drv_gc0308_init;
    _gc0308.open    = drv_gc0308_open;
    _gc0308.close   = drv_gc0308_close;
    _gc0308.read    = drv_gc0308_read;
    _gc0308.write   = drv_gc0308_write;
    _gc0308.control = drv_gc0308_control;
#endif
    _gc0308.user_data = RT_NULL;

    ret = rt_device_register(&_gc0308, "gc0308", RT_DEVICE_FLAG_INT_RX);
    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_gc0308_init);
#endif

/* --- */
int gc0308_reset(sensor_t* sensor)
{
    uint16_t index = 0;
    __gc0308_init();  /* 摄像头初始化 */
	open_gc0308(RGB_CAM);
    return 0;
}

static int gc0308_set_pixformat(sensor_t *sensor, pixformat_t pixformat)
{
	uint8_t data;

	// switch (pixformat)
	// {
	// case PIXFORMAT_RGB565:
	// 	data = 0x26;
	// 	break;
	// case PIXFORMAT_YUV422:
	// case PIXFORMAT_GRAYSCALE:
	// 	data = 0x21;
	// 	break;
	// default:
	// 	return -1;
	// }

	// open_gc0328_0();
	// sensor->write_reg(sensor, 0x44, data);
    // open_gc0328_1();
    // sensor->write_reg(sensor, 0x44, data);

	return 0;
}

static int gc0308_set_framesize(sensor_t *sensor, framesize_t framesize)
{
    int ret=0;
    uint16_t w = resolution[framesize][0];
    uint16_t h = resolution[framesize][1];

    // int i=0;
    // const uint8_t (*regs)[2];

	// if(framesize == FRAMESIZE_QQVGA)
	// {
	// 	regs = qqvga_config;
	// }
	// else if (framesize == FRAMESIZE_240X240){
	// 	regs = B240X240_config;
	// }
    // else if ((w <= 320) && (h <= 240)) {
    //     regs = qvga_config;
    // } else {
    //     regs = vga_config;
    // }

    // while (regs[i][0]) {
    //     cambus_writeb3(sensor->slv_addr, regs[i][0], regs[i][1]);
    //     i++;
    // }
    // /* delay n ms */
    // mp_hal_delay_ms(30);
	dvp_set_image_size(w, h);
    return ret;
}

int gc0308_init(sensor_t *sensor)
{
    //Initialize sensor structure.
    sensor->gs_bpp              = 2;
    sensor->reset               = gc0308_reset;
    // sensor->read_reg            = gc0308_read_reg;
    // sensor->write_reg           = gc0308_write_reg;
    sensor->set_pixformat       = gc0308_set_pixformat;
    sensor->set_framesize       = gc0308_set_framesize;
    // // sensor->set_framerate       = gc0328_set_framerate;
    // sensor->set_contrast        = gc0308_set_contrast;
    // sensor->set_brightness      = gc0308_set_brightness;
    // sensor->set_saturation      = gc0308_set_saturation;
    // sensor->set_gainceiling     = gc0308_set_gainceiling;
    // sensor->set_quality         = gc0308_set_quality;
    // sensor->set_colorbar        = gc0308_set_colorbar;
    // sensor->set_auto_gain       = gc0308_set_auto_gain;
    // sensor->get_gain_db         = gc0308_get_gain_db;
    // sensor->set_auto_exposure   = gc0308_set_auto_exposure;
    // sensor->get_exposure_us     = gc0308_get_exposure_us;
    // sensor->set_auto_whitebal   = gc0308_set_auto_whitebal;
    // sensor->get_rgb_gain_db     = gc0308_get_rgb_gain_db;
    // sensor->set_hmirror         = gc0308_set_hmirror;
    // sensor->set_vflip           = gc0308_set_vflip;
	// sensor->set_windowing       = gc0308_set_windowing;

    // Set sensor flags
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_VSYNC, 0);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_HSYNC, 0);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_PIXCK, 1);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_FSYNC, 0);
    SENSOR_HW_FLAGS_SET(sensor, SENSOR_HW_FLAGS_JPEGE, 1);

    return 0;
}