#include <rtthread.h>
#include <rtdevice.h>

#include "timer.h"
#include "sysctl.h"

struct khwtimer
{
    rt_hwtimer_t time_device;
    timer_device_number_t devno;
    int chn;
    const char *name;
};

extern uint32_t timer_get_count(timer_device_number_t timer_number, timer_channel_number_t channel);
extern void timer_set_reload(timer_device_number_t timer_number, timer_channel_number_t channel, uint32_t count);

static int timer_isr(void *ctx)
{
    struct khwtimer *ktm = (struct khwtimer*)ctx;

    rt_device_hwtimer_isr(&ktm->time_device);

    return 0;
}

static void khwtimer_init(struct rt_hwtimer_device *timer, rt_uint32_t state)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    RT_ASSERT(ktm->devno < TIMER_DEVICE_2);

    if (state)
    {
        sysctl_clock_enable(SYSCTL_CLOCK_TIMER0 + ktm->devno);
    }
    else
    {
        timer_set_enable(ktm->devno, ktm->chn, 0);
    }
}

static rt_err_t khwtimer_start(struct rt_hwtimer_device *timer, rt_uint32_t cnt, rt_hwtimer_mode_t mode)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    RT_ASSERT(ktm->devno < TIMER_DEVICE_2);

    timer_set_reload(ktm->devno, ktm->chn, cnt);
    timer_irq_register(ktm->devno, ktm->chn,
                       mode == HWTIMER_MODE_ONESHOT,
                       1,
                       timer_isr, ktm);

    timer_set_enable(ktm->devno, ktm->chn, 1);

    return 0;
}

static void khwtimer_stop(struct rt_hwtimer_device *timer)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);
    
    RT_ASSERT(ktm->devno < TIMER_DEVICE_2);

    timer_set_enable(ktm->devno, ktm->chn, 0);
}

static rt_uint32_t khwtimer_count_get(struct rt_hwtimer_device *timer)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);

    RT_ASSERT(ktm->devno < TIMER_DEVICE_2);

    return timer_get_count(ktm->devno, ktm->chn);
}

static rt_err_t khwtimer_control(struct rt_hwtimer_device *timer, rt_uint32_t cmd, void *args)
{
    struct khwtimer *ktm = (struct khwtimer*)(timer->parent.user_data);
    rt_uint32_t *f = (rt_uint32_t *)args;

    return 0;
}

static const struct rt_hwtimer_ops _ops =
{
    khwtimer_init,
    khwtimer_start,
    khwtimer_stop,
    khwtimer_count_get,
    khwtimer_control
};

static struct rt_hwtimer_info _info =
{
    .maxcnt  = 0xffffffff,
    .cntmode = HWTIMER_CNTMODE_DW
};

int k210_hwtimer_init(void)
{
    struct khwtimer *ktm;

    _info.maxfreq = sysctl_clock_get_freq(SYSCTL_CLOCK_TIMER0);
    _info.minfreq = _info.maxfreq;

#if  defined(BSP_TIMER0_CHN0_ENABLE) || defined(BSP_TIMER0_CHN1_ENABLE) || defined(BSP_TIMER0_CHN2_ENABLE) || defined(BSP_TIMER0_CHN3_ENABLE)
    timer_init(TIMER_DEVICE_0);
    sysctl_clock_disable(SYSCTL_CLOCK_TIMER0);
#endif

#ifdef BSP_TIMER0_CHN0_ENABLE
    static struct khwtimer _timer00;

    ktm = &_timer00;
    ktm->devno = TIMER_DEVICE_0;
    ktm->chn = TIMER_CHANNEL_0;
    ktm->name = "timer00";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_TIMER0_CHN1_ENABLE
    static struct khwtimer _timer01;

    ktm = &_timer01;
    ktm->devno = TIMER_DEVICE_0;
    ktm->chn = TIMER_CHANNEL_1;     
    ktm->name = "timer01";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_TIMER0_CHN2_ENABLE
    static struct khwtimer _timer02;

    ktm = &_timer02;
    ktm->devno = TIMER_DEVICE_0;
    ktm->chn = TIMER_CHANNEL_2;     
    ktm->name = "timer02";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_TIMER0_CHN3_ENABLE
    static struct khwtimer _timer03;

    ktm = &_timer03;
    ktm->devno = TIMER_DEVICE_0;
    ktm->chn = TIMER_CHANNEL_3;     
    ktm->name = "timer03";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif


#if  defined(BSP_TIMER1_CHN0_ENABLE) || defined(BSP_TIMER1_CHN1_ENABLE) || defined(BSP_TIMER1_CHN2_ENABLE) || defined(BSP_TIMER1_CHN3_ENABLE)
    timer_init(TIMER_DEVICE_1);
    sysctl_clock_disable(SYSCTL_CLOCK_TIMER1);
#endif

#ifdef BSP_TIMER1_CHN0_ENABLE
    static struct khwtimer _timer10;

    ktm = &_timer10;
    ktm->devno = TIMER_DEVICE_1;
    ktm->chn = TIMER_CHANNEL_0;
    ktm->name = "timer10";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_TIMER1_CHN1_ENABLE
    static struct khwtimer _timer11;

    ktm = &_timer11;
    ktm->devno = TIMER_DEVICE_1;
    ktm->chn = TIMER_CHANNEL_1;
    ktm->name = "timer11";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_TIMER1_CHN2_ENABLE
    static struct khwtimer _timer12;

    ktm = &_timer12;
    ktm->devno = TIMER_DEVICE_1;
    ktm->chn = TIMER_CHANNEL_2;
    ktm->name = "timer12";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

#ifdef BSP_TIMER1_CHN3_ENABLE
    static struct khwtimer _timer13;

    ktm = &_timer13;
    ktm->devno = TIMER_DEVICE_1;
    ktm->chn = TIMER_CHANNEL_3;
    ktm->name = "timer13";
    ktm->time_device.ops = &_ops;
    ktm->time_device.info = &_info;

    rt_device_hwtimer_register(&ktm->time_device, ktm->name, ktm);
#endif

    return RT_EOK;
}
INIT_DEVICE_EXPORT(k210_hwtimer_init);
