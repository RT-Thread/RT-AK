#include <rtthread.h>
#include <rtdevice.h>

int micplay(int argc, char **argv)
{
    rt_device_t recdev, playdev;
    rt_uint8_t *buf;
    struct rt_audio_caps caps;
    rt_size_t size;

    recdev = rt_device_find("sound1");
    playdev = rt_device_find("sound0");
    if (!recdev || !playdev)
    {
        rt_kprintf("find sound dev fail\n");
        return -1;
    }

    buf = rt_malloc(1024);
    if (!buf)
    {
        rt_kprintf("no mem\n");
        return -1;
    }

    if (rt_device_open(recdev, RT_DEVICE_OFLAG_RDONLY) != 0)
    {
        rt_kprintf("open record dev fail\n");
        rt_free(buf);

        return -1;
    }

    if (rt_device_open(playdev, RT_DEVICE_OFLAG_WRONLY) != 0)
    {
        rt_free(buf);
        rt_kprintf("open play dev fail\n");
        return -1;
    }

    caps.main_type = AUDIO_TYPE_INPUT;
    caps.sub_type = AUDIO_DSP_PARAM;
    caps.udata.config.samplerate = 8000;
    caps.udata.config.channels = 2;
    caps.udata.config.samplebits = 16;
    rt_device_control(recdev, AUDIO_CTL_CONFIGURE, &caps);

    caps.main_type = AUDIO_TYPE_OUTPUT;
    rt_device_control(playdev, AUDIO_CTL_CONFIGURE, &caps);

    rt_kprintf("always running, reset to stop\n");

    while (1)
    {
        size = rt_device_read(recdev, 0, buf, 1024);
        if (size > 0)
        {
            rt_kprintf("%d\n",*buf);
            rt_device_write(playdev, 0, buf, size);
        }
    }

    return 0;
}
MSH_CMD_EXPORT(micplay, micplay);
