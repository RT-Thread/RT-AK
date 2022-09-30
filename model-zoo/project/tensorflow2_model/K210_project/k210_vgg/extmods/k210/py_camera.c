#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objarray.h"
#include "py/binary.h"

#ifdef EXTMODS_K210_DVP
#include "py_img.h"


typedef struct
{
    mp_obj_base_t base;
    rt_dev_t sensor;
    void *dis;
    void *ai;
    int w;
    int h;
}k210_cam_obj_t;

volatile static uint32_t g_dvp_finish_flag;
static rt_err_t camera_cb(rt_device_t dev, size_t size){
    g_dvp_finish_flag = 1;
    return RT_EOK;
}

static int k210sensor_reset(rt_dev_t *dev){
    if(*dev){
        rt_device_close(*dev);
    }
    *dev = rt_device_find("gc0308");
    if(rt_device_init(*dev) != RT_EOK){
        return -1;
    }
  
    if(rt_device_open(*dev,RT_DEVICE_OFLAG_RDWR) != RT_EOK){
        return -1;
    }
    rt_device_set_rx_indicate(*dev,camera_cb);

    return 0;
}

static int k210sensor_read(rt_dev_t *dev, uint8_t *dis,  uint8_t *ai){
    if(!(*dev)){
        return -1;
    }

    g_dvp_finish_flag = 0;
    rt_device_read(*dev,0,dis,0);

    rt_tick_t start = rt_tick_get();
    while (g_dvp_finish_flag == 0)
    {
        usleep(100);
        if ((rt_tick_get() - start) > rt_tick_from_millisecond(1000)) //wait for 30ms
            return -1;
    }
    return 0;
}

const mp_obj_type_t k210_cam_type;

STATIC mp_obj_t k210_cam_reset(mp_obj_t self)
{
    k210_cam_obj_t *obj = (k210_cam_obj_t *)self;
    if (k210sensor_reset(&obj->sensor) != 0)
    {
        mp_raise_OSError(-1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(k210_cam_reset_obj, k210_cam_reset);
#if 0 
STATIC mp_obj_t k210_cam_set_pixformat(mp_obj_t self, mp_obj_t pixformat)
{
    k210_cam_obj_t *obj = (k210_cam_obj_t *)self;

    if (k210sensor_set_pixformat(&obj->sensor, mp_obj_get_int(pixformat)) != 0)
    {
        mp_raise_OSError(-1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(k210_cam_set_pixformat_obj, k210_cam_set_pixformat);
#endif
#if 1
static mp_obj_t k210_cam_set_framesize(mp_obj_t self, mp_obj_t width, mp_obj_t height) 
{
    k210_cam_obj_t *obj = (k210_cam_obj_t *)self;
    int w, h, ow, oh;

    ow = obj->w;
    oh = obj->h;
    w = mp_obj_get_int(width);
    h = mp_obj_get_int(height);
    if ((w!=320) && (h!=240))
    {
        mp_raise_ValueError("invalid Width or Height, now only support 320x240");
    }
    obj->w = w;
    obj->h = h;
    // if (k210sensor_set_framesize(&obj->sensor, w, h) != 0)
    // {
    //     mp_raise_OSError(-1);
    // }

    if ((w * h) != (ow * oh))
    {
        char *ptr;

        ptr = (char*)realloc(obj->dis, (w * h) * (2 + 3));
        if (!ptr)
        {
            mp_raise_msg_varg(&mp_type_MemoryError, MP_ERROR_TEXT("no memory"));
        }

        obj->dis = ptr;
        obj->ai = ptr + (w * h * 2);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(k210_cam_set_framesize_obj, k210_cam_set_framesize);
#endif

static mp_obj_t k210_cam_snapshot(mp_obj_t self)
{
    k210_cam_obj_t *obj = (k210_cam_obj_t *)self;
    mp_obj_t dis_data, ai_data;
    rt_dev_t *s = &obj->sensor;
    int ret;

    if (!obj->dis)
    {
        mp_raise_ValueError("framebuffer not init");
    }
    
    dis_data = py_img(320, 240, PIXFORMAT_RGB565, 320*240*2, obj->dis);
    ai_data = py_img(320, 240, PIXFORMAT_RGB888_CHW, 320*240*3, obj->ai);
    ret = k210sensor_read(s, obj->dis, obj->ai);
    if (ret < 0) 
    {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_RuntimeError, "Capture Failed: %d", ret));
    }
    mp_obj_list_t *ret_list = NULL;
    ret_list = m_new(mp_obj_list_t, 1);
    mp_obj_list_init(ret_list, 0);
    mp_obj_list_append(ret_list, dis_data );
    mp_obj_list_append(ret_list, ai_data );

    return MP_OBJ_FROM_PTR(ret_list);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(k210_cam_snapshot_obj, k210_cam_snapshot);

static mp_obj_t k210_cam_del(mp_obj_t self)
{
    k210_cam_obj_t *obj = (k210_cam_obj_t *)self;
    if(!obj->sensor){
        rt_device_close(obj->sensor);
        obj->sensor = RT_NULL;
    }
    
    if (obj->dis)
    {
        realloc(obj->dis, 0);
        obj->dis = 0;
        obj->ai = 0;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(k210_cam_del_obj, k210_cam_del);

STATIC mp_obj_t make_new()
{
	k210_cam_obj_t *self = m_new_obj_with_finaliser(k210_cam_obj_t);

	self->base.type = &k210_cam_type;
    self->sensor = RT_NULL;

    return self;
}
#if 0
static mp_obj_t k210_cam_switch(mp_obj_t self, mp_obj_t sel) 
{
    k210_cam_obj_t *obj = (k210_cam_obj_t *)self;
    k210sensor_t *s = &obj->sensor;
    if (mp_obj_get_type(sel) != &mp_type_int)
        mp_raise_ValueError("Invalid Value");
    
    int select = mp_obj_get_int(sel);
    if (select == 1)
        k210sensor_switch(s, 1);
    else if (select == 0)
        k210sensor_switch(s, 0);
    else
        mp_raise_ValueError("Invalid Value");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(k210_cam_switch_obj, k210_cam_switch);
#endif

STATIC const mp_rom_map_elem_t locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&k210_cam_del_obj)},

    {MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&k210_cam_reset_obj)},
    // {MP_ROM_QSTR(MP_QSTR_set_pixformat), MP_ROM_PTR(&k210_cam_set_pixformat_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_framesize), MP_ROM_PTR(&k210_cam_set_framesize_obj)},
    {MP_ROM_QSTR(MP_QSTR_snapshot), MP_ROM_PTR(&k210_cam_snapshot_obj)},
    // {MP_ROM_QSTR(MP_QSTR_sw), MP_ROM_PTR(&k210_cam_switch_obj)},

    // {MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(PIXFORMAT_RGB565)},   /* 2BPP/RGB565*/
    // {MP_ROM_QSTR(MP_QSTR_YUV422), MP_ROM_INT(PIXFORMAT_YUV422)},   /* 2BPP/YUV422*/

};
STATIC MP_DEFINE_CONST_DICT(k210_cam_dict, locals_dict_table);

const mp_obj_type_t k210_cam_type = {
    .base = { &mp_type_type },
    .make_new = make_new,
    .name = MP_QSTR_camera,
    .locals_dict = (mp_obj_t)&k210_cam_dict,
};
#endif
