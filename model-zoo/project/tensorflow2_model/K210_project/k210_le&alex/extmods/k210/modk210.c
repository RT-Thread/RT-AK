#include "py/runtime.h"

#ifdef EXTMODS_K210_FFT
extern const mp_obj_type_t k210_fft_type;
#endif
#ifdef EXTMODS_K210_I2S
extern const mp_obj_type_t k210_i2s_type;
#endif
#ifdef EXTMODS_K210_FPIOA
extern const mp_obj_type_t k210_fpioa_type;
#endif
#ifdef EXTMODS_K210_LCDCTL
extern const mp_obj_type_t k210_lcdctl_type;
#endif
#ifdef EXTMODS_K210_KPU
extern const mp_obj_type_t k210_kpu_type;
#endif
#ifdef EXTMODS_K210_DVP
extern const mp_obj_type_t k210_cam_type;
#endif

#if (defined(EXTMODS_K210_DVP))
extern const mp_obj_type_t py_img_type;
#endif
#ifdef EXTMODS_K210_SHA256
extern const mp_obj_type_t k210_sha256_type;
#endif

STATIC const mp_rom_map_elem_t k210_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_k210) },
#ifdef EXTMODS_K210_FFT
    { MP_ROM_QSTR(MP_QSTR_FFT),  MP_ROM_PTR(&k210_fft_type) },
#endif
#ifdef EXTMODS_K210_I2S
    { MP_ROM_QSTR(MP_QSTR_I2S),  MP_ROM_PTR(&k210_i2s_type) },
#endif
#ifdef EXTMODS_K210_FPIOA
    { MP_ROM_QSTR(MP_QSTR_FPIOA),  MP_ROM_PTR(&k210_fpioa_type) },
#endif
#ifdef EXTMODS_K210_LCDCTL
    { MP_ROM_QSTR(MP_QSTR_lcdctl),  MP_ROM_PTR(&k210_lcdctl_type) },
#endif
#ifdef EXTMODS_K210_KPU
    { MP_ROM_QSTR(MP_QSTR_KPU),  MP_ROM_PTR(&k210_kpu_type) },
#endif
#ifdef EXTMODS_K210_DVP
    { MP_ROM_QSTR(MP_QSTR_camera),  MP_ROM_PTR(&k210_cam_type) },
#endif
#if (defined(EXTMODS_K210_DVP))
    { MP_ROM_QSTR(MP_QSTR_Image),  MP_ROM_PTR(&py_img_type) },
#endif
#ifdef EXTMODS_K210_SHA256
    { MP_ROM_QSTR(MP_QSTR_sha256),  MP_ROM_PTR(&k210_sha256_type) },
#endif
};
STATIC MP_DEFINE_CONST_DICT (k210_module_globals, k210_module_globals_table);

const mp_obj_module_t k210_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&k210_module_globals,
};
