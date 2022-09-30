/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-18     RTT       the first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include    "rtc.h"

#define     HW_RTC_TIME_INIT

static rt_device_t  rtc_dev = RT_NULL;
time_t args = RT_NULL;
struct tm   *time_new;

static int rtc_sample2(void)
{
    rt_err_t ret = RT_EOK;
    rtc_dev = rt_device_find("rtc");
    if(!rtc_dev) {
        rt_kprintf("find rtc device failed\n");
        return RT_ERROR;
    }
    rtc_timer_set(2022, 6, 28, 11, 35, 00);
    // struct tm time_s={
    //     .tm_sec=00,
    //     .tm_min=35,
    //     .tm_hour=11,
    //     .tm_mday=28,
    //     .tm_mon=6,
    //     .tm_year=2022,
    // };
    // time_s.tm_year -= 1900;
    // time_s.tm_mon -= 1;
    // time_t tmp = mktime(&time_s);
    // rt_kprintf("tmp:%d\n",tmp);
    // time_new = gmtime(&tmp);
    // rt_kprintf("%d:%d:%d:%d:%d:%d", (time_new->tm_year + 1900), (time_new->tm_mon+1), time_new->tm_mday, time_new->tm_hour, time_new->tm_min, time_new->tm_sec);
    while(1)
    {
        ret = rt_device_control(rtc_dev, RT_DEVICE_CTRL_RTC_GET_TIME, &args);
        if(ret != RT_EOK) {
            rt_kprintf("get real time fail\n");
            return RT_ERROR;
        }
        // rt_kprintf("%d\n", args);
        time_new = gmtime(&args);
        rt_kprintf("%d:%d:%d:%d:%d:%d\n", (time_new->tm_year + 1900), (time_new->tm_mon+1), time_new->tm_mday, (time_new->tm_hour+RT_LIBC_DEFAULT_TIMEZONE), time_new->tm_min, time_new->tm_sec);
        rt_thread_delay(1000);
    }


    return ret;
}
MSH_CMD_EXPORT(rtc_sample2, rtc get);
