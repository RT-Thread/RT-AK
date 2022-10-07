#include "lcd_wrapper.h"

#include <rtdevice.h>

int omvhal_lcd_init(ovmhal_lcd_t *lcd, int lcdtype, int *o_width, int *o_height)
{
    struct rt_device_graphic_info info;
    int result;

    lcd->dev = rt_device_find("lcd");
    if (!lcd->dev)
        return -1;

    if (rt_device_open(lcd->dev, 0) != 0)
    {
        lcd->dev = 0;
        return -1;
    }

    result = rt_device_control(lcd->dev, RTGRAPHIC_CTRL_GET_INFO, &info);
    if (result != 0)
    {
        rt_device_close(lcd->dev);
        lcd->dev = 0;
        return result;
    }

    if (o_height)
        *o_height = info.height;
    if (o_width)
        *o_width = info.width;
    lcd->fb = info.framebuffer;
    lcd->width = info.width;
    lcd->height = info.height;

    return 0;
}

void omvhal_lcd_deinit(ovmhal_lcd_t *lcd)
{
    if (!lcd || !lcd->dev)
        return;

    rt_device_close(lcd->dev);
    lcd->dev = RT_NULL;
}

void omvhal_lcd_draw(ovmhal_lcd_t *lcd, int x, int y, int w, int h, unsigned short *p)
{
    struct rt_device_rect_info rect;
    uint16_t *fb;
    int i, j;

    if (!lcd || !lcd->dev || !p)
        return;

    fb = (uint16_t *)lcd->fb;
    fb += (lcd->width * y);
    fb += x;

    for (j = 0; j < h; j++)
    {
        for (i = 0; i < w; i++)
        {
            fb[i] = *p;
            p++;
        }
        fb += lcd->width;
    }

    rect.x = x;
    rect.y = y;
    rect.width = w;
    rect.height = h;

    rt_device_control(lcd->dev, RTGRAPHIC_CTRL_RECT_UPDATE, &rect);
}

void omvhal_lcd_clear(ovmhal_lcd_t *lcd, int color)
{
    struct rt_device_rect_info rect;
    uint16_t *fb;
    int i, j;

    if (!lcd || !lcd->dev)
        return;

    fb = (uint16_t *)lcd->fb;

    for (j = 0; j < lcd->height; j++)
    {
        for (i = 0; i < lcd->width; i++)
        {
            fb[i] = (uint16_t)color;
        }
        fb += lcd->width;
    }

    rect.x = 0;
    rect.y = 0;
    rect.width = lcd->width;
    rect.height = lcd->height;

    rt_device_control(lcd->dev, RTGRAPHIC_CTRL_RECT_UPDATE, &rect);
}

void omvhal_set_backlight(ovmhal_lcd_t *lcd, int state)
{
    if (!lcd || !lcd->dev)
        return;

    if (state)
        rt_device_control(lcd->dev, RTGRAPHIC_CTRL_POWERON, RT_NULL);
    else
        rt_device_control(lcd->dev, RTGRAPHIC_CTRL_POWEROFF, RT_NULL);
}
