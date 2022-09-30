/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef __RT_AI_RUNTIME_H__
#define __RT_AI_RUNTIME_H__
#include <rt_ai_def.h>

void run_done(void *arg);
void run_entry(rt_ai_t ai);
rt_ai_record_t rt_ai_record_find(const char *name);
rt_ai_record_t rt_ai_record_register(rt_ai_record_t record, const char *name);
rt_ai_record_t rt_ai_record_unregister(const char *name);
rt_ai_record_t rt_ai_record_create(const char *name, rt_ai_uint32_t value);
rt_ai_record_t rt_ai_record_delete(const char *name);
rt_ai_timestamp_t statistic_time_interval(const char *name);

#define TIMESTAMP_INSERT(_name) \
    rt_ai_record_create(_name, rt_ai_get_timestamp())

#define STATISTIC_TIME_INTERVAL(_name) \
    statistic_time_interval(_name)

#endif //end
