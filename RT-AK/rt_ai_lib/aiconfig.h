/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef __RT_AI_CONFIG__
#define __RT_AI_CONFIG__

/* enable log print */
#define RT_AI_LOG_ENABLE
/* log level & ref rt_ai_log.h */
#define AI_LOG_LEVEL 2
/* RTAK obj name lenght */
#define RT_AI_NAME_MAX 16
/* RTAK suport num of neural model input/output */
#define RT_AI_IO_MAX 10
/* enable print runtime performance (now suport run time cost only) */
#define ENABLE_PERFORMANCE 1
/* use struct rt_ai_record */
#define RT_AI_USE_RECORD 1
//#define AILOG_LEVEL           AILOG_INFO

#define RT_AI_CFG_DEFAULT 0
#define RT_AI_CFG_DMAC 1
#define RT_AI_CFG_CRC 2
#define RT_AI_CFG_KPU 3

//for ai debug
//#define DBG_LEVEL DBG_INFO

#endif

