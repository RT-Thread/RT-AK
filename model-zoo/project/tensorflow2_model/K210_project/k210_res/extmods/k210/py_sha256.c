#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objarray.h"
#include "py/binary.h"

#ifdef EXTMODS_K210_SHA256
#include "sha256.h"

typedef struct 
{
    mp_obj_base_t base;
    sha256_context_t ctx;
} k210_hash_obj_t;

const mp_obj_type_t k210_sha256_type;

STATIC mp_obj_t k210_sha256_update(mp_obj_t self_in, mp_obj_t arg) 
{
    k210_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;

    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    sha256_update(&self->ctx, bufinfo.buf, bufinfo.len);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(k210_sha256_update_obj, k210_sha256_update);

STATIC mp_obj_t k210_sha256_digest(mp_obj_t self_in) 
{
    k210_hash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    vstr_t vstr;

    vstr_init_len(&vstr, SHA256_HASH_LEN);
    sha256_final(&self->ctx, (byte*)vstr.buf);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(k210_sha256_digest_obj, k210_sha256_digest);

STATIC mp_obj_t make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) 
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_int_t len = mp_obj_get_int(args[0]);
    k210_hash_obj_t *o = m_new_obj(k210_hash_obj_t);
    o->base.type = &k210_sha256_type;

    //mp_printf(&mp_plat_print, "input len %d\n", len);
    sha256_init(&o->ctx, len);

    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_rom_map_elem_t locals_dict_table[] = 
{
    {MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&k210_sha256_update_obj)},
    {MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&k210_sha256_digest_obj)},

};
STATIC MP_DEFINE_CONST_DICT(locals_dict, locals_dict_table);

const mp_obj_type_t k210_sha256_type = 
{
    .base = { &mp_type_type },
    .name = MP_QSTR_sha256,
    .make_new = make_new,
    .locals_dict = (void*)&locals_dict,
};
#endif
