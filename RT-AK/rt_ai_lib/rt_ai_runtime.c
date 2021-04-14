/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#include <rt_ai_runtime.h>
#include <rt_ai_common.h>
#include <rt_ai_log.h>
#include <rt_ai_core.h>

void run_entry(rt_ai_t ai)
{
#if ENABLE_PERFORMANCE
    rt_ai_record_t run_cost = rt_ai_record_find("run_time_cost");
    run_cost->record = rt_ai_get_timestamp();
#endif
    return;
}

void run_done(void *arg)
{
    rt_ai_t ai = RT_AI_T(arg);
#if ENABLE_PERFORMANCE
    rt_ai_timestamp_t cost = STATISTIC_TIME_INTERVAL("run_time_cost");
    AI_LOG("run_time_cost:%d\n", cost);
#endif
    // todo
    if (ai->done_callback_user != RT_AI_NULL)
    {
        ai->done_callback_user(ai->arg);
    }
    return;
}

rt_ai_record_t rt_ai_record_find(const char *name)
{
    RT_AI_ASSERT_RETURN_NULL(name);
    rt_ai_core_t core = RT_AI_NULL;
    core = rt_ai_core_find(name,RT_AI_CLASS_RECORD);
    if(core == RT_AI_NULL){
        AI_LOG_I("rt_ai_core_find return RT_AI_NULL in %s,%d",__FILE__,__LINE__);
        return RT_AI_NULL;
    }
    return (rt_ai_record_t) core;
}

rt_ai_record_t rt_ai_record_register(rt_ai_record_t record, const char *name)
{
    RT_AI_ASSERT_RETURN_NULL(record != RT_AI_NULL);
    if (record == RT_AI_NULL)
    {
        return RT_AI_NULL;
    }
    if(rt_ai_record_find("name")){
        AI_LOG_E("record %s exist:%s,%d",__FILE__,__LINE__);
    }
    if(rt_ai_core_register(&(record->parent),RT_AI_CLASS_RECORD,name) == RT_AI_NULL){
        AI_LOG_I("rt_ai_core_register return RT_AI_NULL in %s,%d",__FILE__,__LINE__);
        return RT_AI_NULL;
    }
    return record;
}

rt_ai_record_t rt_ai_record_unregister(const char *name)
{
    rt_ai_record_t record = RT_AI_NULL;
    record = rt_ai_record_find(name);
    if (!record)
    {
        AI_LOG_I("rt_ai_record_find return RT_AI_NULL in %s,%d",__FILE__,__LINE__);
        return RT_AI_NULL;
    }
    rt_ai_core_detach(&(record->parent));
    return record;
}

rt_ai_record_t rt_ai_record_create(const char *name, rt_ai_uint32_t value)
{
    rt_ai_record_t record = rt_ai_new_obj(struct rt_ai_record);
    record->record = value;
    if (rt_ai_record_register(record, name) == RT_AI_NULL)
    {
        AI_LOG_I("rt_ai_record_register return RT_AI_NULL in %s,%d",__FILE__,__LINE__);
        return RT_AI_NULL;
    }
    return record;
    
}

rt_ai_record_t rt_ai_record_delete(const char *name)
{
    rt_ai_record_t record = RT_AI_NULL;
    record = rt_ai_record_unregister(name);
    if (!record)
    {
        AI_LOG_I("rt_ai_record_unregister return RT_AI_NULL in %s,%d",__FILE__,__LINE__);
        return RT_AI_NULL;
    }
    if(!RT_AI_IS_STATICIAL(record->parent.type)){
        AI_LOG("NOT STATICAL\n");
        rt_ai_del(record);
    }
    return record;
}

rt_ai_timestamp_t statistic_time_interval(const char *name)
{
    rt_ai_record_t record = rt_ai_record_find(name);
    if (record == RT_AI_NULL)
    {
        return 0;
    }
    rt_ai_timestamp_t now = rt_ai_get_timestamp();
    return now - record->record;
}

int _rt_ai_runtime_init(){
#if ENABLE_PERFORMANCE
    static struct rt_ai_record run_time_cost;
    run_time_cost.parent.type = RT_AI_STATICIAL_TYPE(0);
    rt_ai_record_register(&run_time_cost,"run_time_cost");
#endif
    return 0;
}
INIT_PREV_EXPORT(_rt_ai_runtime_init);
