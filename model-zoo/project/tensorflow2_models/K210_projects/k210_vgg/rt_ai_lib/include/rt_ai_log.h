/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef __RT_AI_LOG__
#define __RT_AI_LOG__
#include <aiconfig.h>
#include <rtdbg.h>

#undef AI_LOG
#undef AI_LOG_E
#undef AI_LOG_W
#undef AI_LOG_I
#undef ai_log_raw
/* AI LOG level */
#define AI_LOG_ERROR    0
#define AI_LOG_WARNING  1
#define AI_LOG_INFO     2
#define AI_LOG_LOG      3

#ifndef AI_LOG_LEVEL
#define AI_LOG_LEVEL AI_LOG_ERROR
#endif

#define ai_log_raw(...) rt_kprintf(__VA_ARGS__)

#ifdef RT_AI_LOG_ENABLE
#ifdef DBG_ENABLE
#define AI_LOG(_fmt, ...)   LOG_RAW("[AI.LOG]" _fmt, ##__VA_ARGS__)
#define AI_LOG_E(_fmt, ...) LOG_E("[AI.E]" _fmt, ##__VA_ARGS__)
#define AI_LOG_W(_fmt, ...) LOG_W("[AI.W]" _fmt, ##__VA_ARGS__)
#define AI_LOG_I(_fmt, ...) LOG_I("[AI.I]" _fmt, ##__VA_ARGS__)

#else
#define AI_LOG(_fmt, ...)   ai_log_raw("[AI.LOG]" _fmt, ##__VA_ARGS__)
#if (AI_LOG_LEVEL >= AI_LOG_ERROR)
#define AI_LOG_E(fmt, ...)  ai_log_raw("[AI.E]" fmt, ##__VA_ARGS__)
#endif
#if (AI_LOG_LEVEL >= AI_LOG_WARNING)
#define AI_LOG_W(fmt, ...)  ai_log_raw("[AI.W]" fmt, ##__VA_ARGS__)
#endif
#if (AI_LOG_LEVEL >= AI_LOG_INFO)
#define AI_LOG_I(fmt, ...)  ai_log_raw("[AI.I]" fmt, ##__VA_ARGS__)
#endif

#endif //DBG_ENABLE
#endif //RT_AI_LOG_ENABLE

#ifndef AI_LOG
#define AI_LOG(...)
#endif
#ifndef AI_LOG_E
#define AI_LOG_E(...)
#endif
#ifndef AI_LOG_W
#define AI_LOG_W(...)
#endif
#ifndef AI_LOG_I
#define AI_LOG_I(...)
#endif

#define ai_log ai_log_raw

#endif //END

