#ifndef _OMVHALMATH_H
#define _OMVHALMATH_H

#include <math.h>

#define fast_floorf (int)floorf
#define fast_sqrtf sqrtf
#define fast_roundf (int)roundf
#define fast_atan2f atan2f
#define fast_cbrtf cbrtf
#define fast_expf exp
#define fast_powf pow
#define fast_fabsf  fabsf
#define fast_log    log
#define fast_log2   log2
#define fast_ceilf  (int)ceilf
#define fast_atanf  atanf

#define arm_sin_f32 sin
#define arm_cos_f32 cos

#define sinhf sinh

#define __CLZ            __builtin_clz
#define __REV16          __builtin_bswap16                           /* ToDo:  ARMCC_V6: check if __builtin_bswap16 could be used */

#endif
