/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef __BACKEND_K210_KPU_H__
#define __BACKEND_K210_KPU_H__
#include <rtthread.h>
#include "rt_ai.h"
#ifdef RT_AI_USE_K210
#include "kpu.h"
struct k210_kpu
{
    struct rt_ai parent;

    kpu_model_context_t     task;
    uint8_t                 *model;
    dmac_channel_number_t   dmac;
};
typedef struct k210_kpu *k210_kpu_t;
#define K210_KPU_T(h) ((k210_kpu_t)(h))
#define CFG_KPU_DMAC 1

int backend_k210_kpu(void *k210_kpu_s);
#endif //RT_AI_USE_K210
#endif

