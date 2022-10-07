#ifndef __RT_THREAD_DBG_H__
#define __RT_THREAD_DBG_H__

#define DBG_PORT_NAME   "null"

extern uint32_t serial_tx_buf_len(void);
extern void serial_tx_buf_flush(void);
extern void serial_set_irq_enabled(int enabled);
extern size_t serial_tx_buf_read(void *buf, size_t size);
extern size_t serial_tx_buf_write(const char *buf, size_t size);

extern void serial_dbg_deinit(void);
extern void serial_dbg_init(void);

#endif /* __RT_THREAD_DBG_H__ */
