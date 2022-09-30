
/* Copyright Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "wdt.h"

#ifdef  BSP_USING_WDT

static struct rt_watchdog_device wdt0;
static struct rt_watchdog_device wdt1;

static rt_err_t rt_wdt_init(struct rt_watchdog_device * wdt){
    return (RT_EOK);
}

static rt_err_t wdt_control(struct rt_watchdog_device * wdt, int cmd, void * arg){
    rt_uint32_t msecond;
    volatile wdt_t * hw_wdt;
    wdt_device_number_t num = (wdt_device_number_t)wdt->parent.user_data;

    if(num >=WDT_DEVICE_MAX){
        rt_kprintf("wdt number (%d) error\n", num);
    }

    switch(cmd){
    case RT_DEVICE_CTRL_WDT_KEEPALIVE:
        wdt_feed(num);
        break;

    case RT_DEVICE_CTRL_WDT_SET_TIMEOUT:
        msecond = (*((rt_uint32_t *)arg)) * 1000;
        rt_kprintf("wdt timeout (%d)\n",msecond);

        if (msecond != 0){
            wdt_init(num, msecond, RT_NULL, RT_NULL);
            wdt_stop(num);
        }
        else{
            rt_kprintf("wdt timeout value error\n");
        }
        break;

    #if 0
        /* get watchdog timeout */
    case RT_DEVICE_CTRL_WDT_GET_TIMEOUT:
        break;
    #endif
        /* watchdog start */
    case RT_DEVICE_CTRL_WDT_START:
        hw_wdt = (num == WDT_DEVICE_0) ? (volatile wdt_t *)WDT0_BASE_ADDR : (volatile wdt_t *)WDT1_BASE_ADDR;
        hw_wdt->crr = WDT_CRR_MASK;
        hw_wdt->cr |= WDT_CR_ENABLE;
        break;
        /* watchdog stop */
    case RT_DEVICE_CTRL_WDT_STOP:
        wdt_stop(num);
        break;
    default:
        rt_kprintf("This command is not supported.");
        return -RT_ERROR;
    }
    return RT_EOK;
}

int rt_hw_wdt_init(void){
    struct rt_watchdog_ops *ops = rt_malloc(sizeof(struct rt_watchdog_ops));
    if(ops == RT_NULL){
        rt_kprintf("wdt0 device rt_malloc failed\n");
        return -RT_ERROR;
    }

    ops->init = rt_wdt_init;
    ops->control = wdt_control;

    wdt0.ops = ops;

    if (rt_hw_watchdog_register(&wdt0, "wdt0", RT_DEVICE_FLAG_DEACTIVATE, (void *) 0) != RT_EOK){
        rt_kprintf("wdt0 device register failed\n");
        return -RT_ERROR;
    }

    wdt1.ops = ops;

    if (rt_hw_watchdog_register(&wdt1, "wdt1", RT_DEVICE_FLAG_DEACTIVATE, (void *) 1) != RT_EOK){
        rt_kprintf("wdt1 device register failed\n");
        return -RT_ERROR;
    }
    rt_kprintf(" wdt0/1 device register success");
    return RT_EOK;
}

INIT_BOARD_EXPORT(rt_hw_wdt_init);
#endif
