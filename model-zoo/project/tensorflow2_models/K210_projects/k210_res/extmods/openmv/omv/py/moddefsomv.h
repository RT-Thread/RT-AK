#ifndef _MODDEFSOMV_H
#define _MODDEFSOMV_H

extern const struct _mp_obj_module_t sensor_module;
extern const struct _mp_obj_module_t lcd_module;
extern const struct _mp_obj_module_t image_module;
extern const struct _mp_obj_module_t gif_module;
extern const struct _mp_obj_module_t mjpeg_module;
extern const struct _mp_obj_module_t time_module;

#define MPY_MODDEFS_OMV \
    {MP_ROM_QSTR(MP_QSTR_sensor), MP_ROM_PTR(&sensor_module)}, \
    {MP_ROM_QSTR(MP_QSTR_lcd), MP_ROM_PTR(&lcd_module) }, \
    {MP_ROM_QSTR(MP_QSTR_image), MP_ROM_PTR(&image_module) }, \
    {MP_ROM_QSTR(MP_QSTR_gif), MP_ROM_PTR(&gif_module) }, \
    {MP_ROM_QSTR(MP_QSTR_mjpeg), MP_ROM_PTR(&mjpeg_module) }, \
    {MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&time_module) }, \

#endif
