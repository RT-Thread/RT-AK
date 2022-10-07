#include <rtthread.h>
#include <rtdevice.h>
#include "mp.h"
#include "imlib.h"
#include "py/parse.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "rt_thread_dbg.h"
#include "usbdbg.h"

#define DBG_TAG  "DBG"
#define DBG_LVL  DBG_LOG
#include <rtdbg.h>

#define RB_LEN  512

static struct rt_ringbuffer tx_rb;

static uint8_t tx_rb_space[RB_LEN];

static rt_device_t dbg_port = NULL;
rt_sem_t dbg_sem = NULL;

static rt_thread_t tid = NULL;

static void serial_dbg_dispatch(void);

static void serial_dbg_thread_entry(void * param)
{
    while(1) {serial_dbg_dispatch();}
}

void serial_dbg_deinit(void)
{
    if(dbg_port != NULL)
    {
        rt_device_close(dbg_port);
        dbg_port = NULL;
    }
    if(dbg_sem != NULL)
    {
        rt_sem_delete(dbg_sem);
        dbg_sem = NULL;
    }
    if(tid != NULL)
    {
        rt_thread_delete(tid);
        tid = NULL;
    }
}

static rt_err_t dbg_port_rx_ind(rt_device_t dev, rt_size_t size)
{
    if(dbg_sem != NULL)
    {
        rt_sem_release(dbg_sem);
    }
}

void serial_dbg_init(void)
{
    
    dbg_port = rt_device_find(DBG_PORT_NAME);
    if(dbg_port == NULL)
    {
        rt_kprintf("not found device: %s\n", DBG_PORT_NAME);
        return;
    }
    if(rt_device_open(dbg_port, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) != RT_EOK)
    {
        rt_kprintf("open device: %s failed\n", DBG_PORT_NAME);
        RT_ASSERT(0);
    }
    if(dbg_sem == NULL)
    {
        dbg_sem = rt_sem_create("dbg_sem", 0, RT_IPC_FLAG_FIFO);
        RT_ASSERT(dbg_sem);
    }
    //serial_set_irq_enabled(1); //wait system open irq
    rt_device_set_rx_indicate(dbg_port, dbg_port_rx_ind);
    serial_tx_buf_flush();
    if(tid == NULL)
    {
        tid = rt_thread_create("dbg", serial_dbg_thread_entry, NULL, 8192, 17, 10);
        if(tid) {rt_thread_startup(tid);}
    }
    
}
MSH_CMD_EXPORT(serial_dbg_init, serial_dbg_init);

void serial_set_irq_enabled(int enabled)
{
    return;
    if (enabled) {
		rt_device_control(dbg_port, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
    } else {
		rt_device_control(dbg_port, RT_DEVICE_CTRL_CLR_INT, (void *)RT_DEVICE_FLAG_INT_RX);
    }
}

uint32_t serial_tx_buf_len(void)
{
    return rt_ringbuffer_data_len(&tx_rb);
}


void serial_tx_buf_flush(void)
{
    rt_ringbuffer_init(&tx_rb, tx_rb_space, sizeof tx_rb_space);
}

size_t serial_tx_buf_read(void *buf, size_t size)
{
    return rt_ringbuffer_get(&tx_rb, buf, size);
}

size_t serial_tx_buf_write(const char *buf, size_t size)
{
    return rt_ringbuffer_put_force(&tx_rb, buf, size);
}


void serial_dbg_send_strn(const char *str, int len)
{
    serial_tx_buf_write(str, len);
}

void serial_dbg_send_strn_cooked(const char *str, int len)
{
    for (const char *top = str + len; str < top; str++) {
        if (*str == '\n') {
            serial_tx_buf_write((const uint8_t*)"\r\n", 2);
        } else {
            serial_tx_buf_write((const uint8_t*)str, 1);
        }
    }
}

static int serial_recv_block(uint8_t *buf, size_t size, int timeout)
{
    uint32_t xfer_length = size;
    uint32_t offset = 0;
    uint32_t read_size;

    while((xfer_length - offset) > 0)
    {
        if(rt_sem_take(dbg_sem, timeout) == -RT_ETIMEOUT)
        {
            return -1;
        }
        read_size = rt_device_read(dbg_port, 0, &buf[offset], xfer_length - offset);
        offset += read_size;
    }
    
    return size;
}



#define BUFFER_SIZE (64)
static void serial_dbg_dispatch(void)
{
    int ret;
    static uint8_t buf[BUFFER_SIZE] ALIGN(4);

    if ((ret = serial_recv_block(buf, 6, 10)) < 0) 
    {
        return;
    }
    rt_thread_delay(10);
    //ulog_hexdump("rx", 16, buf, 6);
    //mp_printf(&mp_plat_print, "1\n");
    if (ret != 6 || buf[0] != 0x30) 
    {
        return;
    }
    // mp_printf(&mp_plat_print, "2\n");
    uint8_t request = buf[1];
    uint32_t xfer_length = buf[2] | (buf[3] << 8) | (buf[4] << 16) | (buf[5] << 24);
    usbdbg_control(buf+6, request, xfer_length);

    while (xfer_length) {
        if (request & 0x80) {
            // Device-to-host data phase
            int bytes = MIN(xfer_length, BUFFER_SIZE);
            xfer_length -= bytes;
            usbdbg_data_in(buf, bytes);
            rt_device_write(dbg_port, 0, buf, bytes);
            //ulog_hexdump("tx", 16, buf, bytes);
        } else {
            // Host-to-device data phase
            int bytes = MIN(xfer_length, BUFFER_SIZE);

            if ((ret = serial_recv_block(buf, bytes, 500)) < 0) 
            {
                return;
            }
            xfer_length -= ret;
            usbdbg_data_out(buf, ret);
        }
    }
}

