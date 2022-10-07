/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-09-02     liqiwen       the first version
 */

#include <rtthread.h>
#include <stdio.h>
/* Micopython */
#if defined(PKG_USING_MICROPYTHON)
extern void mpy_main(const char *filename);
volatile int msh_run = 0;
int32_t enter_python(uint32_t delay_s)
{
    int i;
    rt_kprintf("type msh to stay msh\n");
    for(i=delay_s; i>0; i--) {
        rt_thread_delay(RT_TICK_PER_SECOND);
        if(msh_run == 1) break;
    }
    while(i==0) {
        mpy_main(NULL);
    }
    return 0;
}

static int msh(int argc, char **argv)
{
    msh_run = 1;
    return 1;
}
MSH_CMD_EXPORT(msh, enter msh skip micropython);

#else
int32_t enter_python(uint32_t delay_s){
    return 0;
}
#endif



