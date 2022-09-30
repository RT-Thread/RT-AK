#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include <rtthread.h>
#ifndef BSP_USING_LCD
/* clang-format off */
#define LCD_X_MAX   (240)
#define LCD_Y_MAX   (320)

#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F
#define USER_COLOR  0xAA55
/* clang-format on */

/* LCD显示方向 */
typedef enum _lcd_dir
{
    DIR_XY_RLUD = 0x00,
    DIR_YX_RLUD = 0x20,
    DIR_XY_LRUD = 0x40,
    DIR_YX_LRUD = 0x60,
    DIR_XY_RLDU = 0x80,
    DIR_YX_RLDU = 0xA0,
    DIR_XY_LRDU = 0xC0,
    DIR_YX_LRDU = 0xE0,
    DIR_XY_MASK = 0x20,
    DIR_MASK = 0xE0,
} lcd_dir_t;

/* LCD结构体 */
typedef struct _lcd_ctl
{
    uint8_t mode;
    uint8_t dir;
    uint16_t width;
    uint16_t height;
} lcd_ctl_t;

void lcd_polling_enable(void);
void lcd_interrupt_enable(void);
void lcd_init(void);
void lcd_clear(uint16_t color);
void lcd_set_direction(lcd_dir_t dir);
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void lcd_set_area(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);
void lcd_draw_string(uint16_t x, uint16_t y, char *str, uint16_t color);
void lcd_draw_picture(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint32_t *ptr);
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t width, uint16_t color);
void lcd_ram_draw_string(char *str, uint32_t *ptr, uint16_t font_color, uint16_t bg_color);
void lcd_draw_picture_half(uint16_t x1, uint16_t y1, uint16_t width, uint16_t height, uint16_t *ptr);
#else //#ifndef BSP_USING_LCD
/* 使用RTT自带LCD驱动,将LCD接口声明与上述一致 */
#include <drv_lcd.h>
#define __DO_WHILE0(_expr) do{ _expr }while(0)
/* for mpy machine.lcd */
// void lcd_display_on(void);
// void lcd_display_off(void);
// void lcd_clear(int color);
// void lcd_set_direction(lcd_dir_t dir);
void lcd_draw_circle(int x1, int y1, int r);
void lcd_set_color(int back, int fore);
#define lcd_draw_point( x,  y, color)  __DO_WHILE0( lcd_draw_point_color((x),  (y), (color)); )
#define lcd_draw_string(x, y, str, color) __DO_WHILE0( lcd_set_color(0x0000, (color)); lcd_show_string((x), (y), 16, (str)); )
#define lcd_draw_line( x1,  y1,  x2,  y2,  color) __DO_WHILE0( lcd_set_color(0x0000, (color)); lcd_draw_line( (x1),  (y1),  (x2),  (y2)); )
#define lcd_draw_rectangle(x1 , y1 , x2, y2, width, color)  __DO_WHILE0( lcd_set_color(0x0000, (color));  lcd_draw_rectangle((x1), (y1) , (x2), (y2)); )
#define lcd_draw_picture(x1, y1, width, height, ptr)  __DO_WHILE0( lcd_show_image((x1), (y1) , (height), (width), (ptr)); )
#endif //BSP_USING_LCD
#endif


