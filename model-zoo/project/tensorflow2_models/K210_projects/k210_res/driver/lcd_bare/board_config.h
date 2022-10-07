#ifndef _BOARD_CONFIG_
#define _BOARD_CONFIG_
#include "fpioa.h"
#include "gpiohs.h"
#include "plic.h"
#include "sysctl.h"
#include "uarths.h"
#include "dvp.h"
#include <rtthread.h>

/* ======> please config following this <====*/
#define  CAMERA_USING_OV5640  0
#define  CAMERA_USING_OV2640  0
#define  CAMERA_USING_GC0308  1

#ifndef BSP_USING_LCD
#define  LCD_USING_NT35310    0
#define  LCD_USING_ST7789     0
#define  LCD_USING_ILI9341    1
#endif

#define  BOARD_KD233        0
#define  BOARD_LICHEEDAN    0
#define  BOARD_K61          0
#define  YB_DKA01           0
#define  EDU_KIT            1
/* ====================================== */
#if CAMERA_USING_OV5640
#define CAMERA "ov5640"
#elif CAMERA_USING_OV2640
#define CAMERA "ov2640"
#elif CAMERA_USING_GC0308
#define CAMERA "gc0308"
#endif

#if CAMERA_USING_OV5640 && CAMERA_USING_OV2640 && CAMERA_USING_GC0308
    #error "CAMERA_USING_XXX be selected >= 2"
#endif
#if LCD_USING_NT35310 && LCD_USING_ST7789
    #error "LCD_USING_XXX be selected 2"
#endif
#if BOARD_KD233 && YB_DKA01
    #error "BOARD be selected 2"
#endif

#if YB_DKA01 || BOARD_LICHEEDAN

/* Camera */
#define DVP_PIN_PCLK           (47)
#define DVP_PIN_XCLK           (46)
#define DVP_PIN_HSYNC          (45)
#define DVP_PIN_PWDN           (44)
#define DVP_PIN_VSYNC          (43)
#define DVP_PIN_RST            (42)
#define DVP_PIN_SCL            (41)
#define DVP_PIN_SDA            (40)

/* LCD */
#define LCD_PIN_CS             (36)
#define LCD_PIN_RST            (37)
#define LCD_PIN_RS             (38)
#define LCD_PIN_WR             (39)

#define LCD_RST_GPIONUM        (3)
#define LCD_RS_GPIONUM         (2)
#define FUNC_LCD_CS             (FUNC_SPI0_SS3)
#define FUNC_LCD_RST            (FUNC_GPIOHS0 + LCD_RST_GPIONUM)
#define FUNC_LCD_RS             (FUNC_GPIOHS0 + LCD_RS_GPIONUM)
#define FUNC_LCD_WR             (FUNC_SPI0_SCLK)

#elif BOARD_KD233
/* Camera */
#define DVP_PIN_PCLK           (15)
#define DVP_PIN_XCLK           (14)
#define DVP_PIN_HSYNC          (17)
#define DVP_PIN_PWDN           (13)
#define DVP_PIN_VSYNC          (12)
#define DVP_PIN_RST            (11)
#define DVP_PIN_SCL            (10)
#define DVP_PIN_SDA            (9)
/* LCD */
//pin
#define LCD_PIN_CS             (6)
// #define LCD_PIN_RST            (37)
#define LCD_PIN_RS             (8)
#define LCD_PIN_WR             (7)
//func
#define FUNC_LCD_CS             (FUNC_SPI0_SS3)
// #define FUNC_LCD_RST            (FUNC_GPIOHS0 + LCD_RST_GPIONUM)
#define FUNC_LCD_RS             (FUNC_GPIOHS0 + 2)
#define FUNC_LCD_WR             (FUNC_SPI0_SCLK)

#elif EDU_KIT
/* Camera */
#define DVP_PIN_PCLK           (8)
#define DVP_PIN_XCLK           (9)
#define DVP_PIN_HSYNC          (10)
#define DVP_PIN_PWDN           (11)
#define DVP_PIN_VSYNC          (12)
#define DVP_PIN_RST            (13)
#define DVP_PIN_SCL            (14)
#define DVP_PIN_SDA            (15)
#define DVP_PIN_SDA0           (0)
/* LCD */
//pin
#define LCD_PIN_CS             (43)
#define LCD_PIN_RST            (41)
#define LCD_PIN_RS             (44)
#define LCD_PIN_WR             (45)
#define LCD_PIN_BACKLIGHT      (47)  
#define LCD_BACKLIGHT_ACTIVE   (1)
//func
#define LCD_BACKLIGHT_GPIONUM  (10)
#define LCD_RST_GPIONUM        (11)
#define LCD_RS_GPIONUM         (12)
#define FUNC_LCD_CS             (FUNC_SPI0_SS3)
#define FUNC_LCD_RST            (FUNC_GPIOHS0 + LCD_RST_GPIONUM)
#define FUNC_LCD_RS             (FUNC_GPIOHS0 + LCD_RS_GPIONUM)
#define FUNC_LCD_WR             (FUNC_SPI0_SCLK)
#define FUNC_LCD_BACKLIGHT      (FUNC_GPIOHS0 + LCD_BACKLIGHT_GPIONUM)

#endif

void io_mux_init(void);
void io_set_power(void);
void camera_init(void);

static inline void __pixel_reversal(uint16_t *pix, int width, int height){ 
    uint16_t tmp;               
    for (int i = 0; i < (width*height);i += 2)  
    { 
        tmp = pix[i];           
        pix[i] = pix[i + 1];            
        pix[i + 1] = tmp;      
    }               
}

#endif
