/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */


#ifndef __RT_AI_COMMON__
#define __RT_AI_COMMON__

#include <rt_ai_def.h>
#include <aiconfig.h>

#define RT_AI_FLAG_INITED   0x01
#define RT_AI_FLAG_LOADED   0X02
#define RT_AI_FLAG_RUN      0x04
#define RT_AI_FLAG_OUTPUT   0x08

#ifndef rt_ai_list_entry
#define rt_ai_list_entry(node, type, member) ((type *)((char *)(node) - (unsigned long)(&((type *)0)->member)))
#endif //rt_list_entry

#ifndef rt_ai_list_for_each
#define rt_ai_list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
#endif //rt_list_for_each

#ifndef rt_ai_list_insert_after
rt_ai_inline void list_insert_after(rt_ai_list_t *l, rt_ai_list_t *n)
{
    n->next = l->next;
    n->prev = l;
    l->next->prev = n;
    l->next = n;
}
#define rt_ai_list_insert_after(l, n) list_insert_after(l, n)
#endif //rt_list_insert_after

#ifndef rt_ai_list_init
rt_ai_inline void list_init(rt_ai_list_t *l)
{
    l->next = l->prev = l;
}
#define rt_ai_list_init(l) list_init(l)
#endif

#ifndef rt_ai_list_remove
rt_ai_inline void list_remove(rt_ai_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}
#define rt_ai_list_remove(l) list_remove(l)
#endif //rt_ai_list_remove


#define RT_AI_ASSERT_RETURN_0(_expr)  \
if (!(_expr))                                                                    \
{                                                                             \
    AI_LOG_E("RT_AI_ASSERT failure:%s , %s , %d\n",#_expr, __FUNCTION__, __LINE__); \
    return -1;  \
}

#define RT_AI_ASSERT_RETURN_NULL(_expr)  \
if (!(_expr))                                                                    \
{                                                                             \
    AI_LOG_E("RT_AI_ASSERT failure:%s , %s , %d\n",#_expr, __FUNCTION__, __LINE__); \
    return RT_AI_NULL;  \
}

#define RT_AI_ASSERT_RETURN(_expr)  \
if (!(_expr))                                                                    \
{                                                                             \
    AI_LOG_E("RT_AI_ASSERT failure:%s , %s , %d\n",#_expr, __FUNCTION__, __LINE__); \
    return ;  \
}

#define ALLOC_WORK_BUFFER_FLAG      0X01
#define ALLOC_INPUT_BUFFER_FLAG     0X02
#define ALLOC_OUTPUT_BUFFER_FLAG    0X04
#define ALLOC_DEFAULT_BUFFER_FLAG   0xffff

#define RT_AI_T(h)                  ((rt_ai_t)(h))
#define RT_AI_INFO(h)               (((rt_ai_t)(h))->info)

#define RT_AI_OUTPUT(_h, _out)      (RT_AI_T(_h)->output[_out])
#define RT_AI_INPUT(_h, _out)       (RT_AI_T(_h)->input[_out])
#define GET_OUTPUT(_h, _out)        RT_AI_OUTPUT(_h, _out)
#define GET_INPUT(_h, _in)          RT_AI_INPUT(_h, _out)
#define GET_NUM_OUTPUT(_h)          (RT_AI_INFO(_h).output_n)
#define GET_NUM_INPUT(_h)           (RT_AI_INFO(_h).input_n)
#define GET_OUTPUT_SIZE(_h, _out)   (RT_AI_INFO(_h).output_n_stack[_out])
#define GET_INPUT_SIZE(_h, _in)     (RT_AI_INFO(_h).input_n_stack[_out])

#define rt_ai_new(_type, _num)      ((_type *)(rt_ai_malloc(sizeof(_type) * (_num))))
#define rt_ai_new_obj(_type)        (rt_ai_new(_type, 1))
#define rt_ai_del(_ptr)             rt_ai_free(_ptr)

void rt_ai_allocate_buffer(rt_ai_t ai, rt_ai_buffer_t *buf);
#endif
