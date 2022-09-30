/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-08-30     liqiwen       the first version
 */
#ifndef MPY_RT_AI_MODEL_PASER_HELPER_H_
#define MPY_RT_AI_MODEL_PASER_HELPER_H_

#include <rtthread.h>

#ifdef MICROPYTHON_USING_USEREXTMODS
#ifdef __cplusplus
extern "C" {
#endif
#include <rt_ai.h>
#include <backend_k210_kpu.h>
rt_ai_t backend_k210_kpu_constructor_helper(void *buffer, char *name);
void backend_k210_kpu_kmodel_free(rt_ai_t handle);
size_t inputs_size(rt_ai_t handle);
size_t outputs_size(rt_ai_t handle);
size_t inputs_n_bytes(rt_ai_t handle, size_t index);
size_t outputs_n_bytes(rt_ai_t handle, size_t index);
#ifdef __cplusplus
}
#endif
#endif //MICROPYTHON_USING_USEREXTMODS
#endif /* MPY_RT_AI_MODEL_PASER_HELPER_H_ */
