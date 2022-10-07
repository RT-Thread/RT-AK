/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef _RT_AI_H
#define _RT_AI_H
#include <rt_ai_def.h>
#include <rt_ai_common.h>

rt_err_t rt_ai_register(rt_ai_t ai, const char *name, rt_uint16_t flags,int (*call)(void *arg), void *arg);
rt_err_t rt_ai_init(rt_ai_t ai, rt_ai_buffer_t* work_buf);
rt_err_t rt_ai_run(rt_ai_t ai, void (*callback)(void * arg), void *arg);
rt_ai_t  rt_ai_find(const char *name);
rt_ai_buffer_t* rt_ai_output(rt_ai_t ai, rt_uint32_t index);
rt_err_t rt_ai_config(rt_ai_t ai, int cmd, rt_ai_buffer_t *arg);
#endif
