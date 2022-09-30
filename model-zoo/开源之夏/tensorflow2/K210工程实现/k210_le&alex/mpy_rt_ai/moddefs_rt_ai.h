/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-28     liqiwen       the first version
 */
#ifndef RT_AI_LIB_MPYTHON_MODDEFS_RT_AI_H_
#define RT_AI_LIB_MPYTHON_MODDEFS_RT_AI_H_

#ifdef MICROPYTHON_USING_USEREXTMODS

extern const struct _mp_obj_module_t rt_ak_module;
#define MICROPY_RT_AI_MODULES \
            {MP_ROM_QSTR(MP_QSTR_rt_ak), MP_ROM_PTR(&rt_ak_module) }, \

#else

#define MICROPY_RT_AI_MODULES

#endif

#endif /* RT_AI_LIB_MPYTHON_MODDEFS_RT_AI_H_ */
