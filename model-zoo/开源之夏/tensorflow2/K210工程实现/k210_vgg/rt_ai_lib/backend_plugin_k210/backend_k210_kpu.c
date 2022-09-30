/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#include <backend_k210_kpu.h>
#include <rt_ai_log.h>
#ifdef RT_AI_USE_K210

#define k210_load_kmodel    kpu_load_kmodel
#define k210_run_kmodel     kpu_run_kmodel
#define k210_get_output     kpu_get_output

extern uint32_t g_ai_done_flag;
static void kpu_done_callback(void *data)
{
    g_ai_done_flag = 1;
}

static int _k210_kpu_init(rt_ai_t ai, rt_ai_buffer_t *buf)
{
    int result = 0;
    ai->input[0] = buf;
    result = k210_load_kmodel(&(K210_KPU_T(ai)->task), (uint8_t *)(K210_KPU_T(ai)->model));
    if (result != 0)
    {
        return -1;
    }
    rt_ai_allocate_buffer(ai, buf);

    return 0;
}

static int _k210_kpu_run(rt_ai_t ai, void (*callback)(void *arg), void *arg)
{
    int result = 0;
    result = k210_run_kmodel(&(K210_KPU_T(ai)->task), (uint8_t *)(ai->input[0]), K210_KPU_T(ai)->dmac, callback, arg);
    if (result != 0)
    {
        return -1;
    }

    return 0;
}

static int _k210_get_ouput(rt_ai_t ai, rt_ai_uint32_t index)
{
    int result = 0;
    size_t size = 0;
    result = kpu_get_output(&(K210_KPU_T(ai)->task), index, (uint8_t **)(ai->output + index), (size_t *)size);
    if (result != 0)
    {
        return -1;
    }

    return 0;
}

static int _k210_get_info(rt_ai_t ai, rt_ai_buffer_t *buf)
{
    return 0;
}

static int _k210_kpu_config(rt_ai_t ai, int cmd, rt_ai_buffer_t *args)
{
    switch (cmd)
    {
    case CFG_KPU_DMAC:
        K210_KPU_T(ai)->dmac = *args;
        break;
    default:
        AI_LOG_W("No cfg\n");
        break;
    }
    return 0;
}

int backend_k210_kpu(void *k210_kpu_s)
{
    RT_AI_T(k210_kpu_s)->init = _k210_kpu_init;
    RT_AI_T(k210_kpu_s)->run = _k210_kpu_run;
    RT_AI_T(k210_kpu_s)->get_output = _k210_get_ouput;
    RT_AI_T(k210_kpu_s)->get_info = _k210_get_info;
    RT_AI_T(k210_kpu_s)->config = _k210_kpu_config;
    RT_AI_T(k210_kpu_s)->mem_flag = ALLOC_INPUT_BUFFER_FLAG;

    return 0;
}

#endif //RT_AI_USE_K210
