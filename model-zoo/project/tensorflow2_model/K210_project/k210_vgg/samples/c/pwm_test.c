#include <rtthread.h>
#include <rtdevice.h>

#include <stdlib.h>

int pwmo(int argc, char **argv)
{
    struct rt_device_pwm *dev;
    int tm = 15 * 1000;
    int chn;
    rt_uint32_t period, pulse;

    if (argc < 5)
    {
        rt_kprintf("Usage: pwmo DEVNAME CHANNEL FREQ DUTY(0~100)\n");
        return -1;
    }

    dev = (struct rt_device_pwm *)rt_device_find(argv[1]);
    if (!dev)
    {
        rt_kprintf("can not find %s\n", argv[1]);
        return -1;
    }

    chn = atoi(argv[2]);
    if (rt_pwm_enable(dev, chn) != 0)
    {
        rt_kprintf("enable channel %s fail\n", argv[2]);
        return -1;
    }

    period = atoi(argv[3]);
    pulse = atoi(argv[4]);

    period = 1000000000 / period;
    pulse = period * pulse / 100;

    if (rt_pwm_set(dev, chn, period, pulse) != 0)
    {
        rt_kprintf("pwm set fail\n");
        goto _out;
    }

    rt_thread_mdelay(tm);

_out:
    rt_pwm_disable(dev, chn);

    return 0;
}
MSH_CMD_EXPORT(pwmo, pwmo DEVNAME CHANNEL FREQ DUTY(0 ~100));
