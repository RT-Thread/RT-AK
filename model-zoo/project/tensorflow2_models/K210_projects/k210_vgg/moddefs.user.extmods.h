#ifndef _MODDEFS_USER_EXTMODS
#define _MODDEFS_USER_EXTMODS

#ifdef PKG_USING_OPENMV_CP
#include "moddefsomv.h"
#else
#define MPY_MODDEFS_OMV
#endif

#ifdef EXTMODS_MISC_USING_K210
#include "moddefsmisc.h"
#else
#define MPY_MODDEFS_MISC
#endif

#ifdef PRJ_USING_RT_AK_EXMODS
#include "moddefs_rt_ai.h"
#else
#define MICROPY_RT_AI_MODULES
#endif
#define MICROPY_USER_MODULES \
    MICROPY_RT_AI_MODULES\
    MPY_MODDEFS_MISC\
    MPY_MODDEFS_OMV\

#define MICROPY_REGISTERED_MODULES \
    {MP_ROM_QSTR(MP_QSTR_uarray), MP_ROM_PTR(&mp_module_uarray)},\

#endif
