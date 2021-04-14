/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */


#include <rt_ai_common.h>
#include <rt_ai_log.h>
void rt_ai_allocate_buffer(rt_ai_t ai, rt_ai_buffer_t *buf)
{
    int addr = 0;
    int i;
    if (ai->mem_flag & ALLOC_WORK_BUFFER_FLAG)
    {
        AI_LOG_I("allocate work offet addr: %d\n", addr);
        ai->workbuffer = buf + addr;
        addr += ai->info.work_buffer_size;
    }
    if (ai->mem_flag & ALLOC_INPUT_BUFFER_FLAG)
    {
        for (i = 0; i < ai->info.input_n; i++)
        {
            AI_LOG_I("allocate in %d offet addr: %d\n", i, addr);
            ai->input[i] = buf + addr;
            addr += ai->info.input_n_stack[i];
        }
    }

    if (ai->mem_flag & ALLOC_OUTPUT_BUFFER_FLAG)
    {
        for (i = 0; i < ai->info.output_n; i++)
        {
            AI_LOG_I("allocate out %d offet addr: %d\n", i, addr);
            ai->output[i] = buf + addr;
            addr += ai->info.output_n_stack[i];
        }
    }
}

