/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef __RT_AI_CORE_H__
#define __RT_AI_CORE_H__
#include <rt_ai_def.h>
#include <rt_ai_common.h>

rt_ai_core_t rt_ai_core_register(struct rt_ai_core *object, enum rt_ai_obj_type type, const char *name);
rt_ai_core_t rt_ai_core_find(const char *name, rt_uint8_t type);
void rt_ai_core_detach(rt_ai_core_t object);
#endif

