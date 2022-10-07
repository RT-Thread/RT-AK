#ifndef __RT_AI_IMG_UTILS_H
#define __RT_AI_IMG_UTILS_H
#include <rtthread.h>
#ifndef IMG_LOG_TAG
#define IMG_LOG_TAG ""
#endif

#define img_log(fmt, ...) rt_kprintf(fmt, ##__VA_ARGS__)
#define IMG_LOG(fmt, ...) img_log( IMG_LOG_TAG fmt "\n", ##__VA_ARGS__)
#define IMG_LOG_ERR(fmt, ...) img_log("[IMG_ERR]" fmt "\n", ##__VA_ARGS__)

#define RET_NONE /* void return */
#define IMG_ASSERT_RET(_expr, _ret) if(!(_expr)){ IMG_LOG_ERR("%s,%d:%s",__FILE__,__LINE__, #_expr); return _ret; }
#endif //__RT_AI_IMG_UTILS_H