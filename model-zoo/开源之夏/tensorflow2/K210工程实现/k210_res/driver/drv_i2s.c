/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 SummerGift <zhangyuan@rt-thread.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "i2s.h"
#include "dmalock.h"

//#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG
#define DBG_TAG "i2s"
#define DBG_COLOR
#include <rtdbg.h>

typedef struct
{
    struct rt_audio_device audio;
    struct rt_audio_configure config;
    struct rt_audio_configure rxconfig;

    const char *name;
    i2s_device_number_t devno;
    dmac_channel_number_t txdma;
    i2s_device_number_t rxdevno;
    dmac_channel_number_t rxdma;
    uint8_t *tx_fifo;
    uint16_t *tx_buf;
    uint32_t *rx_buf;
    int rxindex;
    uint32_t caps;
} ki2s_t;

#define KI2S_TX_BLKSZ 1024
#define KI2S_RX_BLKSZ 512

static int dmacb(void *ctx)
{
    ki2s_t *ki2s = (ki2s_t *)ctx;

    rt_audio_tx_complete(&ki2s->audio);

    return 0;
}

static int rx_dmacb(void *ctx)
{
    ki2s_t *i2s = (ki2s_t *)ctx;
    int16_t *d;
    uint32_t *rec, *tmp;
    int i;

    if (i2s->rxindex)
    {
        i2s->rxindex = 0;
        rec = i2s->rx_buf;
        d = (int16_t*)&i2s->rx_buf[KI2S_RX_BLKSZ];
    }
    else
    {
        i2s->rxindex = 1;
        rec = &i2s->rx_buf[KI2S_RX_BLKSZ];
        d = (int16_t*)i2s->rx_buf;
    }

    i2s_receive_data_dma(i2s->rxdevno, (void*)rec, KI2S_RX_BLKSZ, i2s->rxdma);

    tmp = (uint32_t*)d;
    for (i = 0; i < KI2S_RX_BLKSZ/2; i ++)
    {
        d[2 * i] = (int16_t)(tmp[2 * i + 1]&0xffff);
        d[2 * i + 1] = d[2 * i];
    }

    rt_audio_rx_done(&i2s->audio, (rt_uint8_t*)d, KI2S_RX_BLKSZ * 2);

    return 0;
}

static void i2s_send_16_32(i2s_device_number_t device_num, dmac_channel_number_t channel_num,
                           const uint8_t *buf, size_t buf_len, size_t bits_per_sample)
{
    size_t sample_cnt;

    sample_cnt = buf_len / (bits_per_sample / 8) / 2;

    if (bits_per_sample == 16)
    {
        i2s_set_dma_divide_16(device_num, 1);
        i2s_send_data_dma(device_num, buf, sample_cnt, channel_num);
    }
    else if (bits_per_sample == 32)
    {
        i2s_set_dma_divide_16(device_num, 0);
        i2s_send_data_dma(device_num, buf, sample_cnt * 2, channel_num);
    }
}

static int voice_to16(uint8_t *pcm, size_t length, int track_num, int bps, uint16_t *buf, size_t *send_len)
{
    int i, j = 0;

    switch (bps)
    {
    case 16:
    {
        uint16_t *src = (uint16_t *)pcm;

        length /= 2;
        for (i = 0; i < length; i++)
        {
            buf[2 * i] = src[i];
            buf[2 * i + 1] = 0;
        }
    }
    break;
    case 24:
    {
        uint8_t *src = pcm;

        length /= 3;
        for (i = 0; i < length; i++)
        {
            buf[2 * i] = 0;
            buf[2 * i] |= src[j++];
            buf[2 * i] |= src[j++] << 8;
            j++;

            buf[2 * i + 1] = 0;
            if (track_num == 2)
            {
                buf[2 * i + 1] |= pcm[j++];
                buf[2 * i + 1] |= pcm[j++] << 8;
                j++;
            }
        }
    }
    break;
    case 32:
    default:
    {
        uint32_t *src = (uint32_t *)pcm;

        length /= 4;
        for (i = 0; i < length; i++)
        {
            buf[2 * i] = (uint16_t)src[i];
            buf[2 * i + 1] = 0;
        }
    }
    break;
    }

    *send_len = length * 2 * 2;

    return 0;
}

static void ki2s_send(ki2s_t *i2s, void *buf, size_t len)
{
    int track_num = i2s->config.channels;
    int bps = i2s->config.samplebits;

    if (((bps == 16) || (bps == 32)) && (track_num == 2))
    {
        i2s_send_16_32(i2s->devno, i2s->txdma, (const uint8_t *)buf, len, bps);
    }
    else
    {
        voice_to16(buf, len, track_num, bps, i2s->tx_buf, &len);
        i2s_send_16_32(i2s->devno, i2s->txdma, (const uint8_t *)i2s->tx_buf, len, 16);
    }
}

static void i2s_block_seten(i2s_device_number_t device_num, i2s_transmit_t rxtx_mode, int en)
{
    irer_t u_irer;
    iter_t u_iter;

    if (rxtx_mode == I2S_RECEIVER)
    {
        u_irer.reg_data = readl(&i2s[device_num]->irer);
        u_irer.irer.rxen = en;
        writel(u_irer.reg_data, &i2s[device_num]->irer);
        /* Receiver block disable */
    }
    else
    {
        u_iter.reg_data = readl(&i2s[device_num]->iter);
        u_iter.iter.txen = en;
        writel(u_iter.reg_data, &i2s[device_num]->iter);
        /* Transmitter block disable */
    }
}

static rt_err_t _i2s_getcaps(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    ki2s_t *i2s;

    RT_ASSERT(audio != RT_NULL);
    RT_ASSERT(caps != RT_NULL);

    i2s = (ki2s_t *)audio;

    switch (caps->main_type)
    {
    case AUDIO_TYPE_QUERY:
        switch (caps->sub_type)
        {
        case AUDIO_TYPE_QUERY:
            caps->udata.mask = i2s->caps;
            break;
        default:
            result = -RT_ERROR;
            break;
        } // switch (caps->sub_type)
        break;

    case AUDIO_TYPE_MIXER:
    {
        switch (caps->sub_type)
        {
        case AUDIO_MIXER_QUERY:
            return -1;

        default:
            return -1;
        }
    }
    break;
    case AUDIO_TYPE_INPUT:
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
            caps->udata.config.samplerate = i2s->rxconfig.samplerate;
            caps->udata.config.channels = i2s->rxconfig.channels;
            caps->udata.config.samplebits = i2s->rxconfig.samplebits;
            break;
        case AUDIO_DSP_SAMPLERATE:
            caps->udata.config.samplerate = i2s->rxconfig.samplerate;
            break;
        case AUDIO_DSP_CHANNELS:
            caps->udata.config.channels = i2s->rxconfig.channels;
            break;
        case AUDIO_DSP_SAMPLEBITS:
            caps->udata.config.samplebits = i2s->rxconfig.samplebits;
            break;
        default:
            result = -RT_ERROR;
            break;
        }
        break;
    case AUDIO_TYPE_OUTPUT:
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
            caps->udata.config.samplerate = i2s->config.samplerate;
            caps->udata.config.channels = i2s->config.channels;
            caps->udata.config.samplebits = 16;
            break;
        case AUDIO_DSP_SAMPLERATE:
            caps->udata.config.samplerate = i2s->config.samplerate;
            break;
        case AUDIO_DSP_CHANNELS:
            caps->udata.config.channels = i2s->config.channels;
            break;
        case AUDIO_DSP_SAMPLEBITS:
            caps->udata.config.samplebits = 16;
            break;
        default:
            result = -RT_ERROR;
            break;
        }
        break;
    }

    return result;
}

static rt_err_t _i2s_configure(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    ki2s_t *i2s;
    int stream = -1;

    RT_ASSERT(audio != RT_NULL);
    RT_ASSERT(caps != RT_NULL);

    i2s = (ki2s_t *)audio;

    LOG_D("config mtype(%d) stype(%d)", caps->main_type, caps->sub_type);

    switch (caps->main_type)
    {
    case AUDIO_TYPE_MIXER:
        break;
    case AUDIO_TYPE_INPUT:
    {
        stream = AUDIO_STREAM_RECORD;
        if (!(i2s->caps & AUDIO_TYPE_INPUT))
            return -RT_EIO;

        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
        {
            i2s->rxconfig.samplebits = caps->udata.config.samplebits;
            i2s->rxconfig.samplerate = caps->udata.config.samplerate;
            i2s_set_sample_rate(i2s->rxdevno, caps->udata.config.samplerate);
        }
        break;
        case AUDIO_DSP_SAMPLERATE:
        {
            i2s->rxconfig.samplerate = caps->udata.config.samplerate;
            i2s_set_sample_rate(i2s->rxdevno, caps->udata.config.samplerate);
        }
        break;
        case AUDIO_DSP_CHANNELS:
            break;
        case AUDIO_DSP_SAMPLEBITS:
            break;
        default:
            result = -RT_ERROR;
            break;
        }
    }
    break;
    case AUDIO_TYPE_OUTPUT:
    {
        rt_bool_t bNeedReset = RT_FALSE;

        if (stream < 0)
            stream = AUDIO_STREAM_REPLAY;

        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
        {
            i2s_tx_channel_config(i2s->devno, I2S_CHANNEL_1,
                                  RESOLUTION_16_BIT, SCLK_CYCLES_32,
                                  TRIGGER_LEVEL_4, RIGHT_JUSTIFYING_MODE);

            i2s->config.channels = caps->udata.config.channels;
            i2s->config.samplebits = caps->udata.config.samplebits;
            i2s->config.samplerate = caps->udata.config.samplerate;
            i2s_set_sample_rate(i2s->devno, caps->udata.config.samplerate);
        }
        break;
        case AUDIO_DSP_SAMPLEBITS:
        {
            i2s->config.samplebits = caps->udata.config.samplebits;
        }
        break;
        case AUDIO_DSP_CHANNELS:
        {
            i2s->config.channels = caps->udata.config.channels;
        }
        break;
        case AUDIO_DSP_SAMPLERATE:
        {
            i2s->config.samplerate = caps->udata.config.samplerate;
            i2s_set_sample_rate(i2s->devno, caps->udata.config.samplerate);
        }
        break;
        default:
            result = -RT_ERROR;
            break;
        }

        if (bNeedReset)
        {
            return 0;
        }
    }
    break;
    default:
        result = -RT_ERROR;
        break;
    }

    return result;
}

static rt_err_t _i2s_init(struct rt_audio_device *audio)
{
    rt_err_t result = RT_EOK;
    ki2s_t *i2s;

    RT_ASSERT(audio != RT_NULL);

    i2s = (ki2s_t *)audio;

    if (i2s->caps & AUDIO_TYPE_OUTPUT)
    {
        i2s_init(i2s->devno, I2S_TRANSMITTER, 0x0C);
        i2s_tx_channel_config(i2s->devno, I2S_CHANNEL_1,
                          RESOLUTION_16_BIT, SCLK_CYCLES_32,
                          TRIGGER_LEVEL_4, RIGHT_JUSTIFYING_MODE);

        i2s->config.channels = 2;
        i2s->config.samplebits = 16;
        i2s->config.samplerate = 8000;
        i2s_set_sample_rate(i2s->devno, i2s->config.samplerate);
    }

    if (i2s->caps & AUDIO_TYPE_INPUT)
    {
        i2s->rxindex = 0;
        i2s_init(i2s->rxdevno, I2S_RECEIVER, 0x3);
        i2s_rx_channel_config(i2s->rxdevno, I2S_CHANNEL_0,
                              RESOLUTION_16_BIT, SCLK_CYCLES_32,
                              TRIGGER_LEVEL_4, STANDARD_MODE);

        i2s->rxconfig.channels = 2;
        i2s->rxconfig.samplebits = 16;
        i2s->rxconfig.samplerate = 8000;
        i2s_set_sample_rate(i2s->rxdevno, i2s->rxconfig.samplerate);
    }

    return result;
}

static rt_err_t _i2s_start(struct rt_audio_device *audio, int stream)
{
    ki2s_t *i2s;

    RT_ASSERT(audio != RT_NULL);

    i2s = (ki2s_t *)audio;

    switch (stream)
    {
    case AUDIO_STREAM_REPLAY:
    {
        if (!(i2s->caps & AUDIO_TYPE_OUTPUT))
            return -RT_EIO;
        if (i2s->txdma == DMAC_CHANNEL_MAX)
        {
            if (dmalock_sync_take(&i2s->txdma, 2000))
            {
                LOG_E("Fail to take DMA channel");
                return -RT_EIO;
            }
        }
        dmac_irq_register(i2s->txdma, dmacb, i2s, 1);
        i2s_block_seten(i2s->devno, I2S_TRANSMITTER, 1);
        ki2s_send(i2s, i2s->tx_fifo, 1024);

        LOG_I("Start replay.[%X]", i2s);
    }
    break;
    case AUDIO_STREAM_RECORD:
    {
        if (!(i2s->caps & AUDIO_TYPE_INPUT))
            return -RT_EIO;
        if (i2s->rxdma == DMAC_CHANNEL_MAX)
        {
            if (dmalock_sync_take(&i2s->rxdma, 2000))
            {
                LOG_E("Fail to take DMA channel");
                return -RT_EIO;
            }
        }
        dmac_set_irq(i2s->rxdma, rx_dmacb, i2s, 4);
        i2s_block_seten(i2s->rxdevno, I2S_RECEIVER, 1);
        i2s_receive_data_dma(i2s->rxdevno, (void*)i2s->rx_buf, KI2S_RX_BLKSZ, i2s->rxdma);

        LOG_I("Start record.");
    }
    break;
    }

    return RT_EOK;
}

static rt_size_t _i2s_transmit(struct rt_audio_device *audio, const void *writeBuf, void *readBuf, rt_size_t size)
{
    ki2s_t *i2s;

    RT_ASSERT(audio != RT_NULL);

    i2s = (ki2s_t *)audio;

    ki2s_send(i2s, (void *)writeBuf, size);

    return size;
}

static rt_err_t _i2s_stop(struct rt_audio_device *audio, int stream)
{
    ki2s_t *i2s;

    RT_ASSERT(audio != RT_NULL);

    i2s = (ki2s_t *)audio;

    switch (stream)
    {
    case AUDIO_STREAM_REPLAY:
    {
        if (!(i2s->caps & AUDIO_TYPE_OUTPUT))
            return -RT_EIO;

        i2s_block_seten(i2s->devno, I2S_TRANSMITTER, 0);

        if (i2s->txdma != DMAC_CHANNEL_MAX)
        {
            dmalock_release(i2s->txdma);
            i2s->txdma = DMAC_CHANNEL_MAX;
        }
        LOG_I("Stop replay.");
    }
    break;
    case AUDIO_STREAM_RECORD:
    {
        if (!(i2s->caps & AUDIO_TYPE_INPUT))
            return -RT_EIO;

        i2s_block_seten(i2s->rxdevno, I2S_RECEIVER, 0);

        if (i2s->rxdma != DMAC_CHANNEL_MAX)
        {
            dmalock_release(i2s->rxdma);
            i2s->rxdma = DMAC_CHANNEL_MAX;
        }
        LOG_I("Stop record.");
    }
    break;
    default:
        return -RT_EINVAL;
    }

    return RT_EOK;
}

static void _i2s_buffer_info(struct rt_audio_device *audio, struct rt_audio_buf_info *info)
{
    ki2s_t *i2s;

    RT_ASSERT(audio != RT_NULL);
    RT_ASSERT(info != RT_NULL);

    i2s = (ki2s_t *)audio;

    info->buffer = i2s->tx_fifo;
    info->block_count = 2;
    info->total_size = KI2S_TX_BLKSZ * info->block_count;
    info->block_size = KI2S_TX_BLKSZ;
}

static const struct rt_audio_ops _ops =
{
    .getcaps = _i2s_getcaps,
    .configure = _i2s_configure,

    .init = _i2s_init,
    .start = _i2s_start,
    .stop = _i2s_stop,
    .transmit = _i2s_transmit,
    .buffer_info = _i2s_buffer_info
};

#ifdef BSP_USING_I2S0
static ki2s_t _i2s0 =
{
    .name = "sound0",
    .devno = I2S_DEVICE_0,
    .txdma = DMAC_CHANNEL_MAX,
    .caps = AUDIO_TYPE_OUTPUT | AUDIO_TYPE_MIXER,
};
#endif

#ifdef BSP_USING_I2S1
static ki2s_t _i2s1 =
{
    .name = "sound1",
    .rxdevno = I2S_DEVICE_1,
    .rxdma = DMAC_CHANNEL_MAX,
    .caps = AUDIO_TYPE_INPUT | AUDIO_TYPE_MIXER,
};
#endif

#ifdef BSP_USING_I2S2
static ki2s_t _i2s2 =
{
    .name = "sound2",
    .devno = I2S_DEVICE_2,
    .txdma = DMAC_CHANNEL_MAX,
    .caps = AUDIO_TYPE_OUTPUT | AUDIO_TYPE_MIXER,
};
#endif

static int ki2s_meminit(ki2s_t *i2s)
{
    size_t len = 0;
    uint8_t *ptr;

    if (i2s->caps & AUDIO_TYPE_OUTPUT)
        len += ((KI2S_TX_BLKSZ * 2) + (KI2S_TX_BLKSZ * sizeof(uint32_t) * 2));
    if (i2s->caps & AUDIO_TYPE_INPUT)
        len += (KI2S_RX_BLKSZ * sizeof(uint32_t) * 2);

    ptr = (uint8_t*)rt_malloc(len);
    if (!ptr)
        return -1;

    if (i2s->caps & AUDIO_TYPE_OUTPUT)
    {
        i2s->tx_fifo = ptr;
        ptr += (KI2S_TX_BLKSZ * 2);
        i2s->tx_buf = (uint16_t *)ptr;
    }

    if (i2s->caps & AUDIO_TYPE_INPUT)
    {
        i2s->rx_buf = (uint32_t*)ptr;
    }

    return 0;
}

int rt_hw_i2s_init(void)
{
    ki2s_t *i2s;

#ifdef BSP_USING_I2S0
    i2s = &_i2s0;

    if (ki2s_meminit(i2s) != 0)
    {
        LOG_E("no mem");
        return -1;
    }

    i2s->audio.ops = (struct rt_audio_ops *)&_ops;
    rt_audio_register(&i2s->audio, i2s->name, RT_DEVICE_FLAG_WRONLY, i2s);
#endif

#ifdef BSP_USING_I2S1
    i2s = &_i2s1;

    if (ki2s_meminit(i2s) != 0)
    {
        LOG_E("no mem");
        return -1;
    }

    i2s->audio.ops = (struct rt_audio_ops *)&_ops;
    rt_audio_register(&i2s->audio, i2s->name, RT_DEVICE_FLAG_RDONLY, i2s);
#endif

#ifdef BSP_USING_I2S2
    i2s = &_i2s2;

    if (ki2s_meminit(i2s) != 0)
    {
        LOG_E("no mem");
        return -1;
    }

    i2s->audio.ops = (struct rt_audio_ops *)&_ops;
    rt_audio_register(&i2s->audio, i2s->name, RT_DEVICE_FLAG_RDWR, i2s);
#endif

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_i2s_init);
