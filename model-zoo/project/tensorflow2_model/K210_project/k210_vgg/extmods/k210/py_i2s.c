#include <stdio.h>
#include <string.h>

#include "py/binary.h"
#include "py/objarray.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"

#ifdef EXTMODS_K210_I2S
#include "i2s.h"
#include "dmac.h"
#include "rtdevice.h"
#define RX_FRAME_LEN 512

typedef struct
{
    mp_obj_base_t base;
    int devno;
    int workmode;
    int txdma;
    int rxdma;
    int bps;
    int track_num;
    int sample_rate;
    int rxtx_mode;
    uint32_t *rxbuf;
    rt_device_t dev;
} k210_i2s_obj_t;

const mp_obj_type_t k210_i2s_type;

STATIC void k210_i2s_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    k210_i2s_obj_t *i2s = (k210_i2s_obj_t *)self_in;

    mp_printf(print, "I2S: device(%d)\n", i2s->devno);
    mp_printf(print, "   align mode: %d\n", i2s->workmode);
    mp_printf(print, "   sample rate: %d\n", i2s->sample_rate);
    mp_printf(print, "   track num: %d\n", i2s->track_num);
    mp_printf(print, "   bits per sample: %d\n", i2s->bps);
}

STATIC mp_obj_t k210_i2s_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    char name[8];

    mp_arg_check_num(n_args, n_kw, 2, MP_OBJ_FUN_ARGS_MAX, true);

    mp_int_t devno = mp_obj_get_int(args[0]);
    if (devno >= I2S_DEVICE_MAX)
    {
        mp_raise_ValueError("I2S:invalid device");
    }
    rt_sprintf(name, "sound%d", devno);

    mp_int_t rxtxmode = mp_obj_get_int(args[1]);
    if ((rxtxmode != I2S_TRANSMITTER) && (rxtxmode != I2S_RECEIVER))
    {
        mp_raise_ValueError("I2S:invalid rxtx_mode");
    }

    k210_i2s_obj_t *self = m_new_obj(k210_i2s_obj_t);

    self->base.type = &k210_i2s_type;
    self->devno = devno;
    self->rxtx_mode = rxtxmode;
    self->dev = rt_device_find(name);
    if (!self->dev)
    {
        mp_raise_ValueError("I2S:dev not found");
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t k210_i2s_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    k210_i2s_obj_t *i2s = (k210_i2s_obj_t *)pos_args[0];

    if ((i2s->rxtx_mode == I2S_TRANSMITTER) && (rt_device_open(i2s->dev, RT_DEVICE_OFLAG_WRONLY) != 0))
    {
        mp_raise_OSError(-1);
    }

    if ((i2s->rxtx_mode == I2S_RECEIVER) && (rt_device_open(i2s->dev, RT_DEVICE_OFLAG_RDONLY) != 0))
    {
        mp_raise_OSError(-1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(k210_i2s_init_obj, 0, k210_i2s_init);

STATIC mp_obj_t k210_i2s_deinit(mp_obj_t self_in)
{
    k210_i2s_obj_t *i2s = (k210_i2s_obj_t *)self_in;

    rt_device_close(i2s->dev);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(k210_i2s_deinit_obj, k210_i2s_deinit);

STATIC mp_obj_t k210_i2s_set_param(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    k210_i2s_obj_t *i2s = (k210_i2s_obj_t *)pos_args[0];
    struct rt_audio_caps caps;

    enum
    {
        ARG_sample_rate,
        ARG_bps,
        ARG_track_num,
    };
    static const mp_arg_t allowed_args[] =
        {
            {MP_QSTR_sample_rate, MP_ARG_INT, {.u_int = 8000}},
            {MP_QSTR_bps, MP_ARG_INT, {.u_int = -1}},
            {MP_QSTR_track_num, MP_ARG_INT, {.u_int = -1}},
        };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (i2s->rxtx_mode == I2S_TRANSMITTER)
        caps.main_type = AUDIO_TYPE_OUTPUT;
    else
        caps.main_type = AUDIO_TYPE_INPUT;
    caps.sub_type = AUDIO_DSP_PARAM;

    rt_device_control(i2s->dev, AUDIO_CTL_GETCAPS, &caps);

    caps.udata.config.samplerate = args[ARG_sample_rate].u_int;
    if (args[ARG_bps].u_int != -1)
        caps.udata.config.samplebits = args[ARG_bps].u_int;
    if (args[ARG_track_num].u_int != -1)
        caps.udata.config.channels = args[ARG_track_num].u_int;

    rt_device_control(i2s->dev, AUDIO_CTL_CONFIGURE, &caps);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(k210_i2s_set_param_obj, 1, k210_i2s_set_param);

STATIC mp_obj_t k210_i2s_play(mp_obj_t self_in, mp_obj_t pcm)
{
    k210_i2s_obj_t *i2s = (k210_i2s_obj_t *)self_in;

    mp_buffer_info_t bufinfo;

    mp_get_buffer_raise(pcm, &bufinfo, MP_BUFFER_READ);

    rt_device_write(i2s->dev, 0, bufinfo.buf, bufinfo.len);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(k210_i2s_play_obj, k210_i2s_play);

STATIC mp_obj_t k210_i2s_record(mp_obj_t self_in)
{
    k210_i2s_obj_t *i2s = (k210_i2s_obj_t *)self_in;

    int16_t *buf = m_new(int16_t, RX_FRAME_LEN);
    mp_obj_array_t *out = m_new_obj(mp_obj_array_t);

    out->base.type = &mp_type_bytearray;
    out->typecode = BYTEARRAY_TYPECODE;
    out->free = 0;
    out->len = RX_FRAME_LEN * sizeof(uint16_t);
    out->items = (void*)buf;

    rt_device_read(i2s->dev, 0, buf, out->len);

    return out;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(k210_i2s_record_obj, k210_i2s_record);

STATIC const mp_rom_map_elem_t k210_i2s_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&k210_i2s_deinit_obj)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&k210_i2s_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_param), MP_ROM_PTR(&k210_i2s_set_param_obj)},
    {MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&k210_i2s_play_obj)},
    {MP_ROM_QSTR(MP_QSTR_record), MP_ROM_PTR(&k210_i2s_record_obj)},
    {MP_ROM_QSTR(MP_QSTR_TRANSMITTER), MP_ROM_INT(I2S_TRANSMITTER)},
    {MP_ROM_QSTR(MP_QSTR_RECEIVER), MP_ROM_INT(I2S_RECEIVER)},
#if 0
    {MP_ROM_QSTR(MP_QSTR_STANDARD_MODE), MP_ROM_INT(STANDARD_MODE)},
    {MP_ROM_QSTR(MP_QSTR_RIGHT_JUSTIFYING_MODE), MP_ROM_INT(RIGHT_JUSTIFYING_MODE)},
    {MP_ROM_QSTR(MP_QSTR_LEFT_JUSTIFYING_MODE), MP_ROM_INT(LEFT_JUSTIFYING_MODE)},
#endif
};
STATIC MP_DEFINE_CONST_DICT(k210_i2s_dict, k210_i2s_locals_dict_table);

const mp_obj_type_t k210_i2s_type = {
    {&mp_type_type},
    .name = MP_QSTR_I2S,
    .print = k210_i2s_print,
    .make_new = k210_i2s_make_new,
    .locals_dict = (mp_obj_dict_t *)&k210_i2s_dict,
};
#endif
