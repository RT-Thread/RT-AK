#include <math.h>
#include <rtthread.h>
#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objarray.h"
#include "py/binary.h"

#ifdef EXTMODS_K210_FFT
#include "dmalock.h"
#include <ffth.h>

typedef struct _k210_fft_obj_t
{
	mp_obj_base_t base;
} k210_fft_obj_t;

const mp_obj_type_t k210_fft_type;

STATIC mp_obj_t fft_run(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    size_t nitems = 0;
    const mp_obj_t *items = 0;
    enum
    {
        ARG_input,
        ARG_shift,
        ARG_direction,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_input, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        {MP_QSTR_shift, MP_ARG_INT, {.u_int = 0}},
        {MP_QSTR_direction, MP_ARG_INT, {.u_int = FFT_DIR_FORWARD}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint32_t shift = args[ARG_shift].u_int;
    uint32_t direction = args[ARG_direction].u_int;

    if (args[ARG_input].u_obj == mp_const_none)
    {
        mp_raise_ValueError("FFT:invalid input");
    }

    mp_obj_get_array(args[ARG_input].u_obj, &nitems, (mp_obj_t **)&items);

    if (nitems != 64 && nitems != 128 && nitems != 256 && nitems != 512)
    {
        mp_raise_ValueError("FFT:invalid points");
    }

    //    mp_printf(&mp_plat_print, "%X,%X\n", shift, direction);

    fft_data_t *fdin, *fdout;

    fdin = m_new(fft_data_t, nitems * 2);
    fdout = fdin + nitems;

    for (int i = 0; i < nitems / 2; i++)
    {
        fdin[i].R1 = mp_obj_get_int(*items++);
        fdin[i].I1 = 0;
        fdin[i].R2 = mp_obj_get_int(*items++);
        fdin[i].I2 = 0;

        //        mp_printf(&mp_plat_print, "<%d,%d>\n", fdin[i].R1, fdin[i].R2);
    }

    //return a list
    mp_obj_list_t *ret_list = m_new(mp_obj_list_t, 1);
    mp_obj_list_init(ret_list, 0);

    dmac_channel_number_t send = DMAC_CHANNEL_MAX, recv = DMAC_CHANNEL_MAX;

    if (dmalock_sync_take(&send, 2000))
        goto _err1;
    
    if (dmalock_sync_take(&recv, 2000))
        goto _err2;
    
    fft_complex_uint16_dma(DMAC_CHANNEL3, DMAC_CHANNEL4, shift, direction, (uint64_t *)fdin, nitems, (uint64_t *)fdout);
    
    dmalock_release(send);
    dmalock_release(recv);

    mp_obj_t tuple_1[2];

    for (int i = 0; i < nitems / 2; i++)
    {
        tuple_1[0] = mp_obj_new_int(fdout->R1);
        tuple_1[1] = mp_obj_new_int(fdout->I1);
        mp_obj_list_append(ret_list, mp_obj_new_tuple(MP_ARRAY_SIZE(tuple_1), tuple_1));

        tuple_1[0] = mp_obj_new_int(fdout->R2);
        tuple_1[1] = mp_obj_new_int(fdout->I2);
        mp_obj_list_append(ret_list, mp_obj_new_tuple(MP_ARRAY_SIZE(tuple_1), tuple_1));
        fdout++;
    }

    return MP_OBJ_FROM_PTR(ret_list);

_err2:
    dmalock_release(send);
_err1:
    mp_raise_msg_varg(&mp_type_OSError, "Fail to take DMA channel");

}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(fft_run_obj, 0, fft_run);

STATIC mp_obj_t fft_make_new()
{
	k210_fft_obj_t *self = m_new_obj(k210_fft_obj_t);
	self->base.type = &k210_fft_type;

	return self;
}

STATIC const mp_rom_map_elem_t fft_locals_dict_table[] = {
    {MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&fft_run_obj)},

    {MP_ROM_QSTR(MP_QSTR_DIR_BACKWARD), MP_ROM_INT(FFT_DIR_BACKWARD)},
    {MP_ROM_QSTR(MP_QSTR_DIR_FORWARD), MP_ROM_INT(FFT_DIR_FORWARD)},
};
STATIC MP_DEFINE_CONST_DICT(k210_fft_dict, fft_locals_dict_table);

const mp_obj_type_t k210_fft_type = {
    {&mp_type_type},
    .name = MP_QSTR_FFT,
    .make_new = fft_make_new,
    .locals_dict = (mp_obj_dict_t *)&k210_fft_dict,
};
#endif
