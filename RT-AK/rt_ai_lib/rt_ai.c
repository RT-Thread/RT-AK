/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#include <rt_ai_runtime.h>
#include <rt_ai.h>
#include <rt_ai_log.h>
#include <rt_ai_core.h>
#ifdef RT_USING_AI_OPS
#define ai_init     (ai->ops->init)
#define ai_load     (ai->ops->load)
#define ai_run      (ai->ops->run)
#define ai_output   (ai->ops->get_output)
#define ai_get_info (ai->ops->get_info)
#define ai_config   (ai->ops->config)
#else
#define ai_init     (ai->init)
#define ai_input    (ai->get_input)
#define ai_run      (ai->run)
#define ai_output   (ai->get_output)
#define ai_get_info (ai->get_info)
#define ai_config   (ai->config)
#endif


/**
 * This function finds a ai handle by specified name.
 *
 * @param name the ai handle's name
 *
 * @return the registered ai handle on successful, or RT_AI_NULL on failure.
 */
rt_ai_t rt_ai_find(const char *name)
{
    RT_AI_ASSERT_RETURN_NULL(name);
    rt_ai_core_t core = NULL;
    core = rt_ai_core_find(name,RT_AI_CLASS_HANDLE);
    if(core == NULL){
        AI_LOG_I("rt_ai_core_find return NULL %s,%d!\n",__FILE__,__LINE__);
        return RT_AI_NULL;
    }
    return (rt_ai_t) core;
}

/**
 * This function registers a ai handle with specified name.
 *
 * @param ai the pointer of ai handle structure
 * @param name the ai handle's name
 * @param flags Temporary unused
 * @param call backend construct function
 * @param arg arg of backend construct function
 *
 * @return the error code, RT_AI_OK on register successfully.
 */
rt_err_t rt_ai_register(rt_ai_t ai, const char *name, rt_uint16_t flags, int (*call)(void *arg), void *arg)
{
    RT_AI_ASSERT_RETURN_0(ai != RT_AI_NULL);
    if (ai == RT_AI_NULL)
        return -RT_AI_ERROR;
    if (call(arg) != RT_AI_OK)
    {
        AI_LOG_E("register callback err! %s,%d\n",__FILE__,__LINE__);
        return -RT_AI_ERROR;
    }
    AI_LOG_I("register model %s\n", name);
    if(rt_ai_core_register(&(ai->parent),RT_AI_CLASS_STATIC_HANDLE,name) == NULL){
        AI_LOG_E("rt_ai_core_register err!%s,%d\n",__FILE__,__LINE__);
    }
    return RT_AI_OK;
}

/**
 * This function removes a previously registered ai handle
 *
 * @param ai the pointer of ai handle structure
 *
 * @return the error code, RT_AI_OK on successfully.
 */
rt_err_t rt_ai_unregister(rt_ai_t ai)
{
    //todo
    return RT_AI_OK;
}


/**
 * This function will initialize the specified ai & backend & memery of runtime
 *
 * @param ai the pointer of ai handle structure
 * @param work_buf runtime memery
 *
 * @return the result
 */
rt_err_t rt_ai_init(rt_ai_t ai, rt_ai_buffer_t *work_buf)
{
    RT_AI_ASSERT_RETURN_0(ai != RT_AI_NULL);
    RT_AI_ASSERT_RETURN_0(ai_init != RT_AI_NULL);
    rt_err_t result = RT_AI_OK;
    ai->flag = 0;
    /* get ai_init handler */
    ai->workbuffer = work_buf;
    if (ai_init == RT_AI_NULL)
    {
        AI_LOG_E("ai init interface is None!\n");
        return RT_AI_ERROR;
    }

    result = ai_init(ai, work_buf);
    if (result != RT_AI_OK)
    {
        AI_LOG_E("ai init interface return a err!\n", result);
        return result;
    }

    ai->flag |= RT_AI_FLAG_INITED;

    return RT_AI_OK;
}

/**
 * This function will get a input addr of ai
 *
 * @param ai the pointer of ai handle structure
 * @param index the index of input of ai
 *
 * @return addr if get input success; RT_AI_NULL if fail
 */
rt_ai_buffer_t *rt_ai_input(rt_ai_t ai, rt_ai_uint32_t index)
{

    RT_AI_ASSERT_RETURN_NULL(ai != RT_AI_NULL);
    rt_err_t result = RT_AI_OK;

    /* call ai_open interface */
    if (ai_input == RT_AI_NULL)
    {
        AI_LOG_E("ai input interface is None!\n");
        return RT_AI_NULL;
    }
    result = ai_input(ai, index);
    /* set open flag */
    if (result != RT_AI_OK)
    {
        AI_LOG_E("ai input interface return a err!\n", result);
        return RT_AI_NULL;
    }
    return ai->input[index];
}

/**
 * This function will run a ai
 *
 * @param ai the pointer of ai handle structure
 * @param callback the callback func of run complete
 * @param arg the arg of callback func
 *
 * @return the result
 */
rt_err_t rt_ai_run(rt_ai_t ai, void (*callback)(void *arg), void *arg)
{
    RT_AI_ASSERT_RETURN_0(ai != RT_AI_NULL);
    RT_AI_ASSERT_RETURN_0(ai_run != RT_AI_NULL);
    rt_err_t result = RT_AI_OK;

    /* if ai is not initialized, initialize it. */
    if (!(ai->flag & RT_AI_FLAG_INITED))
    {
        AI_LOG_E("ai uninitialize!\n");
        return -RT_AI_ERROR;
    }

    /* call ai_close interface */
    if (ai_run == RT_AI_NULL)
    {
        AI_LOG_E("ai run interface is None!\n");
        return RT_AI_ERROR;
    }

    ai->done_callback_user = callback;
    ai->arg = arg;

    run_entry(ai);
    result = ai_run(ai, run_done, (void *)ai);

    /* set open flag */
    if (result != RT_AI_OK)
    {
        AI_LOG_E("ai run interface return a err!\n");
        return result;
    }
    ai->flag |= RT_AI_FLAG_RUN;
    ai->flag &= ~RT_AI_FLAG_OUTPUT;

    return RT_AI_OK;
}

/**
 * This function will get a output addr of ai
 *
 * @param ai the pointer of ai handle structure
 * @param index the index of output of ai
 *
 * @return addr if get input success; RT_AI_NULL if fail
 */
rt_ai_buffer_t *rt_ai_output(rt_ai_t ai, rt_ai_uint32_t index)
{
    RT_AI_ASSERT_RETURN_NULL(ai != RT_AI_NULL);
    RT_AI_ASSERT_RETURN_NULL(ai_output != RT_AI_NULL);

    if (!(ai->flag & RT_AI_FLAG_RUN) | !(ai->flag & RT_AI_FLAG_INITED))
    {
        AI_LOG_E("ai uninitialize or not run\n");
        return RT_AI_NULL;
    }

    if (ai_output == RT_AI_NULL)
    {
        AI_LOG_E("ai output interface is None!\n");
        return RT_AI_NULL;
    }
    /* call ai_read interface */
    int result = 0;
    result = ai_output(ai, index);
    if (result != 0)
    {
        AI_LOG_E("ai output interface return a err!\n");
        return RT_AI_NULL;
        //        ai->output_size = *size;
    }
    ai->flag |= RT_AI_FLAG_OUTPUT;

    return ai->output[index];
}


/**
 * This function will print the info of ai.
 *
 * @param ai the pointer of ai handle structure
 *
 * @return the err code.
 *
 */
rt_err_t rt_ai_info(rt_ai_t ai)
{
    RT_AI_ASSERT_RETURN_0(ai != RT_AI_NULL);

    if (!(ai->flag & RT_AI_FLAG_INITED))
    {
        AI_LOG_E("error: uninitialize!");
        return -RT_AI_ERROR;
    }

    ai_log("**********model info:************\n");
    ai_log("%-15s:%d\n", "input num: ", ai->info.input_n);
    ai_log("%-15s:%d\n", "output num: ", ai->info.output_n);
    ai_log("%-15s:%d\n", "work_buf size: ", ai->info.work_buffer_size);
    for (int i = 0; i < RT_AI_INFO(ai).input_n; i++)
    {
        ai_log("%-15s%d: %d\n", "input size:", i, RT_AI_INFO(ai).input_n_stack[i]);
    }
    for (int i = 0; i < RT_AI_INFO(ai).output_n; i++)
    {
        ai_log("%-15s%d: %d\n", "output size", i, RT_AI_INFO(ai).output_n_stack[i]);
    }

    return RT_AI_OK;
}


/**
 * This function will perform a variety of cofig functions on ai.
 *
 * @param ai the pointer of ai handle structure
 * @param cmd the command sent to ai
 * @param arg the argument of command
 *
 * @return the result
 */
rt_err_t rt_ai_config(rt_ai_t ai, int cmd, rt_ai_buffer_t *arg)
{
    RT_AI_ASSERT_RETURN_0(ai != RT_AI_NULL);
    RT_AI_ASSERT_RETURN_0(ai_config != RT_AI_NULL);
    int result = 0;
    /* call ai_write interface */
    result = ai_config(ai, cmd, arg);
    if (result != RT_AI_OK)
    {
        return result;
    }

    return RT_AI_OK;
}


