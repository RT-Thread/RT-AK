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

#include    <rtthread.h>
#include    <rtdevice.h>
#include    "rtc.h"

#ifdef      BSP_USING_ONCHIP_RTC
#define     HW_RTC_TIME_INIT

static struct rt_device rtc_dev;

static rt_err_t rt_hw_rtc_control(rt_device_t dev, int cmd, void *args)
{
    time_t      *time;
    struct tm   *time_new;
    struct tm    time_s;

    RT_ASSERT(dev != RT_NULL);

    switch (cmd)
    {
    /* read time */
    case RT_DEVICE_CTRL_RTC_GET_TIME:
        time = (time_t *)args;

        rtc_timer_get(&time_s.tm_year,
                      &time_s.tm_mon,
                      &time_s.tm_mday,
                      &time_s.tm_hour,
                      &time_s.tm_min,
                      &time_s.tm_sec);

        time_s.tm_year -= 1900;
        time_s.tm_mon -= 1;
        *time = mktime(&time_s);
        break;

    /* set time */
    case RT_DEVICE_CTRL_RTC_SET_TIME:
        time = (time_t *)args;
        time_new = gmtime(time);

        rtc_init();
        rtc_timer_set(time_new->tm_year + 1900,
                     time_new->tm_mon + 1,
                     time_new->tm_mday,
                     time_new->tm_hour,
                     time_new->tm_min,
                     time_new->tm_sec);
        break;

    default:
        break;
    }
    return RT_EOK;
}

int rt_hw_rtc_init(void)
{
    #if defined(HW_RTC_TIME_INIT)
    rtc_init();
    rtc_timer_set(2021, 3, 15, 17, 40, 00);
    #endif

    /* register rtc device */
    rtc_dev.type        = RT_Device_Class_RTC;
    rtc_dev.init        = RT_NULL;
    rtc_dev.open        = RT_NULL;
    rtc_dev.close       = RT_NULL;
    rtc_dev.read        = RT_NULL;
    rtc_dev.write       = RT_NULL;
    rtc_dev.control     = rt_hw_rtc_control;
    rtc_dev.user_data   = RT_NULL;

    rt_device_register(&rtc_dev, "rtc", RT_DEVICE_FLAG_RDWR);

    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_init);

#endif
