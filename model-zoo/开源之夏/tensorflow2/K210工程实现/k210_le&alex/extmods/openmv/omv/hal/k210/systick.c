#include "systick.h"
#include "sleep.h"
#include <rtthread.h>

void systick_sleep(int ms)
{
    msleep(ms);
}

uint32_t systick_current_millis(void)
{
    double t;
    uint32_t ms;

    t = rt_tick_get();
    ms = (uint32_t)((t / RT_TICK_PER_SECOND) * 1000);

    return ms;
}
