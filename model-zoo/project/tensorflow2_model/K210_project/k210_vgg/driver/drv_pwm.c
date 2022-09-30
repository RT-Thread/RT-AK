#include    <rtthread.h>
#include    <rtdevice.h>
#include    "pwm.h"

#ifdef      BSP_USING_PWM

typedef struct {
    struct rt_device_pwm pwm;
    pwm_device_number_t devno;
    uint32_t chnmask;
} kpwm_t;

static rt_err_t _pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    struct rt_pwm_configuration *cfg = (struct rt_pwm_configuration*)arg;
    kpwm_t *kpwm = (kpwm_t*)device;

    if (!(kpwm->chnmask & (1<<cfg->channel))) {
        rt_kprintf("[err] mchl:%x cchl:%x\n", kpwm->chnmask, cfg->channel);
        return -RT_EIO;
    }

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        pwm_set_enable(kpwm->devno, cfg->channel, 1);
        break;
    case PWM_CMD_DISABLE:
        pwm_set_enable(kpwm->devno, cfg->channel, 0);
        break;
    case PWM_CMD_SET:
        {
        double freq, duty = 0;

        freq = 1000000000/(double)cfg->period;
        if (cfg->pulse)
        {
            duty = (double)100/((double)cfg->period/cfg->pulse);
            duty = duty/100;
        }

        pwm_set_frequency(kpwm->devno, cfg->channel, freq, duty);
        }
        break;
    case PWM_CMD_GET:
        break;
    default:
        return -RT_EINVAL;
    }
    return RT_EOK;
}

static const struct rt_pwm_ops _ops =
{
    .control = _pwm_control,
};

int rt_hw_pwm_init(void)
{
    static kpwm_t _kpwm;
    kpwm_t *kpwm;

    kpwm = &_kpwm;
    kpwm->devno = PWM_DEVICE_2;
    kpwm->chnmask = 0;

#ifdef BSP_PWM_CHN0_ENABLE
    kpwm->chnmask |= (1<<PWM_CHANNEL_0);
#endif
#ifdef BSP_PWM_CHN1_ENABLE
    kpwm->chnmask |= (1<<PWM_CHANNEL_1);
#endif
#ifdef BSP_PWM_CHN2_ENABLE
    kpwm->chnmask |= (1<<PWM_CHANNEL_2);
#endif
#ifdef BSP_PWM_CHN3_ENABLE
    kpwm->chnmask |= (1<<PWM_CHANNEL_3);
#endif
    
    pwm_init(kpwm->devno);
    rt_device_pwm_register(&kpwm->pwm, "pwm", &_ops, kpwm);
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_pwm_init);

#endif
/*pwm_output freq_max= 1mhz*/
