/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-29     liqiwen       the first version
 */

#include <rtthread.h>

#ifdef MICROPYTHON_USING_USEREXTMODS

#include <rt_ai.h>

#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"

#include <backend_k210_kpu.h>
#include "nncase.h"

rt_ai_t backend_k210_kpu_constructor_helper(void *buffer, char *name){

    // backend_k210_kpu
    k210_kpu_t new_k210_kpu = m_new_obj(struct k210_kpu);
    new_k210_kpu->model = buffer;
    new_k210_kpu->dmac = DMAC_CHANNEL5;

    // rt_ai_t
    rt_ai_t handle = &(new_k210_kpu->parent);
    handle->info.input_n = 0;
    handle->info.output_n = 0;
    handle->info.flag = ALLOC_INPUT_BUFFER_FLAG;
    rt_strncpy(handle->parent.name, name, RT_AI_NAME_MAX);
    backend_k210_kpu((void*)new_k210_kpu);

    return handle;
}

void backend_k210_kpu_kmodel_free(rt_ai_t handle){
    kpu_model_free(&(((k210_kpu_t)handle)->task));
    handle->flag = 0;
    return;
}

size_t inputs_size(rt_ai_t handle){
    kpu_model_context_t _ctx = ((k210_kpu_t)handle)->task;
    kpu_model_context_t *ctx = &_ctx;
    return km_inputs_size(ctx);
}

size_t outputs_size(rt_ai_t handle){
    kpu_model_context_t _ctx = ((k210_kpu_t)handle)->task;
    kpu_model_context_t *ctx = &_ctx;
    return km_outputs_size(ctx);
}

size_t inputs_n_bytes(rt_ai_t handle, size_t index){
    kpu_model_context_t _ctx = ((k210_kpu_t)handle)->task;
    kpu_model_context_t *ctx = &_ctx;
    return km_inputs_n_bytes(ctx, index);
}

size_t outputs_n_bytes(rt_ai_t handle, size_t index){
    kpu_model_context_t _ctx = ((k210_kpu_t)handle)->task;
    kpu_model_context_t *ctx = &_ctx;
    return km_outputs_n_bytes(ctx, index);
}

#endif
