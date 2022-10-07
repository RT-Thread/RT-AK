#ifndef __MODDEFSMISC_H
#define __MODDEFSMISC_H

#ifdef EXTMODS_MISC_USING_K210
extern const struct _mp_obj_module_t k210_module;
#define MPY_MODDEFS_K210 {MP_ROM_QSTR(MP_QSTR_k210), MP_ROM_PTR(&k210_module) },
#else
#define MPY_MODDEFS_K210
#endif

#define MPY_MODDEFS_MISC \
    MPY_MODDEFS_K210 \

#endif
