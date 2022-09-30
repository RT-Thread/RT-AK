/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#ifndef __RT_AI_DEF__
#define __RT_AI_DEF__
#include <rtthread.h>
#include <aiconfig.h>

#define RT_AI_NULL          NULL
#define RT_AI_OK            0
#define RT_AI_ERROR         1
#define RT_AI_EREADY        2
#define RT_AI_EBUSY         3
#define RT_AI_NONE          4
#define RT_AI_EEMPTY        5

#define rt_ai_inline                            rt_inline
#define RT_AI_INLINE                            rt_ai_inline
#define RT_AI_UNUSED(x)                         RT_UNUSED(X);
#define RT_AI_ASSERT(_expr)                     RT_ASSERT(_expr)
#define rt_ai_malloc(_size)                     rt_malloc(_size)
#define rt_ai_free(_ptr)                        rt_free(_ptr)
#define rt_ai_list_t                            rt_list_t
#define rt_ai_timestamp_t                       rt_tick_t
#define rt_ai_get_timestamp()                   rt_tick_get()
#define rt_ai_list_entry(node, type, member)    rt_list_entry(node, type, member)
#define rt_ai_list_for_each(pos, head)          rt_list_for_each(pos, head)
#define rt_ai_list_insert_after(l, n)           rt_list_insert_after(l, n)
#define rt_ai_list_init(l)                      rt_list_init(l)

typedef rt_uint8_t  rt_ai_buffer_t;
typedef rt_uint32_t rt_ai_uint32_t;
typedef rt_uint16_t rt_ai_uint16_t;


#define RT_AI_STATICIAL_TYPE(_type) (RT_AI_CLASS_STATIC | _type)
#define RT_AI_IS_STATICIAL(_type)   (RT_AI_CLASS_STATIC & _type)
#define RT_AI_MASK_STATICIAL(_type) ((~RT_AI_CLASS_STATIC) & _type)

/*
 * define object type info for the number of rt_ai_core_container items.
 */
enum rt_ai_obj_type
{
    RT_AI_CLASS_HANDLE = 0, /**< The type is a rt_ai. */
#ifdef RT_AI_USE_RECORD
    RT_AI_CLASS_RECORD, /**< The object is a record. */
#endif
    RT_AI_CLASS_UNKNOWN, /**< The object is unknown. */

    RT_AI_CLASS_STATIC = 0X80, /** RT_AI STATICAL TYPE */
    RT_AI_CLASS_STATIC_HANDLE   = RT_AI_STATICIAL_TYPE(RT_AI_CLASS_HANDLE),
#ifdef RT_AI_USE_RECORD /**< The type is a statical rt_ai. */
    RT_AI_CLASS_STATIC_RECORD   = RT_AI_STATICIAL_TYPE(RT_AI_CLASS_RECORD),
#endif /**< The type is a statical record. */
    RT_AI_CLASS_STATIC_UNKNOWN  = RT_AI_STATICIAL_TYPE(RT_AI_CLASS_UNKNOWN), /**< The type is a statical unknow. */
};

/**
 * Base structure of Kernel object
 */
struct rt_ai_core
{
    char name[RT_AI_NAME_MAX]; /**< name of kernel object */
    rt_uint8_t type;           /**< type of kernel object */
    rt_uint8_t flag;           /**< flag of kernel object */
    rt_list_t  list;            /**< list node of kernel object */
};
typedef struct rt_ai_core *rt_ai_core_t; /**< Type for kernel objects. */


typedef struct rt_ai_info *rt_ai_info_t;
struct rt_ai_info
{
    rt_ai_uint32_t input_n;
    rt_ai_uint32_t output_n;
    rt_ai_uint32_t input_n_stack[RT_AI_IO_MAX];
    rt_ai_uint32_t output_n_stack[RT_AI_IO_MAX];
    rt_ai_uint32_t work_buffer_size;
    rt_uint16_t    flag;
};

typedef struct rt_ai *rt_ai_t;

/**
 * ai handle structure
 */
struct rt_ai
{
    struct rt_ai_core   parent;
    rt_ai_uint16_t      flag; /**< ai flag */
    rt_ai_uint16_t      mem_flag;
    rt_ai_buffer_t      *workbuffer;           /** runtime buf */
    rt_ai_buffer_t      *input[RT_AI_IO_MAX];  /* num of input */
    rt_ai_buffer_t      *output[RT_AI_IO_MAX]; /* num of output */
    struct rt_ai_info   info;
    void                *cfg; /* platform config */

#ifdef RT_USING_AI_OPS
    const struct rt_ai_ops *ops;
#else
    /* common ai interface 
     *@return 0:OK; other:fail
    */
    int (*init)         (rt_ai_t ai, rt_ai_buffer_t *buf);
    int (*get_input)    (rt_ai_t ai, rt_ai_uint32_t index);
    int (*run)          (rt_ai_t ai, void (*callback)(void *arg), void *arg);
    int (*get_output)   (rt_ai_t ai, rt_ai_uint32_t index);
    int (*get_info)     (rt_ai_t ai, rt_ai_buffer_t *buf);
    int (*config)       (rt_ai_t ai, int cmd, rt_ai_buffer_t *args);

#endif
    /* callback when run done */
    void (*done_callback_user)(void *arg);
    void *arg;
};

/**
 * operations set for ai object
 */
typedef struct _rt_ai_ops
{
    /* common ai interface */
    // todo
    rt_uint16_t flag;
} rt_ai_ops;

typedef struct rt_ai_record *rt_ai_record_t;
struct rt_ai_record
{
    struct rt_ai_core      parent;
    rt_ai_uint32_t  record;
};

#endif
