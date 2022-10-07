/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-04-01     liqiwen      the first version
 */

#include<rt_ai_core.h>
#include<rt_ai_log.h>
/**
 * The information of the kernel object
 */
struct rt_ai_core_information
{
    enum rt_ai_obj_type type; /**< object class type */
    rt_list_t object_list;    /**< object list */
    rt_size_t object_size;    /**< object size */
};

#define _RT_AI_CONTAINER_LIST_INIT(c)  { &(rt_ai_core_container[c].object_list), &(rt_ai_core_container[c].object_list) }
static struct rt_ai_core_information rt_ai_core_container[RT_AI_CLASS_UNKNOWN] =
{
    /* initialize object container - rt_ai_handle */
    {RT_AI_CLASS_HANDLE, _RT_AI_CONTAINER_LIST_INIT(RT_AI_CLASS_HANDLE), sizeof(struct rt_ai)},
#ifdef RT_AI_USE_RECORD
    /* initialize object container - rt_ai_record */
    {RT_AI_CLASS_RECORD, _RT_AI_CONTAINER_LIST_INIT(RT_AI_CLASS_RECORD), sizeof(struct rt_ai_record)},
#endif
};

/**
 * This function will return the specified type of object information.
 *
 * @param type the type of object, which can be 
 *             RT_Object_Class_Thread/Semaphore/Mutex... etc
 *
 * @return the object type information or RT_AI_NULL
 */
struct rt_ai_core_information *rt_ai_core_get_information(enum rt_ai_obj_type type)
{
    int index;
    for (index = 0; index < RT_AI_CLASS_UNKNOWN; index++)
    {
        if (RT_AI_MASK_STATICIAL(rt_ai_core_container[index].type) == RT_AI_MASK_STATICIAL(type))
        {
            return &rt_ai_core_container[index];
        }
    }
    return RT_AI_NULL;
}

/**
 * This function will find specified name object from object
 * container.
 *
 * @param name the specified name of object.
 * @param type the type of object
 *
 * @return the found object or RT_AI_NULL if there is no this object
 * in object container.
 *
 * @note this function shall not be invoked in interrupt status.
 */
rt_ai_core_t rt_ai_core_find(const char *name, rt_uint8_t type)
{
    struct rt_ai_core *object = RT_AI_NULL;
    struct rt_list_node *node = RT_AI_NULL;
    struct rt_ai_core_information *information = RT_AI_NULL;

    information = rt_ai_core_get_information((enum rt_ai_obj_type)type);

    /* parameter check */
    if ((name == RT_AI_NULL) || (information == RT_AI_NULL)) return RT_AI_NULL;

    /* try to find object */
    rt_list_for_each(node, &(information->object_list))
    {
        object = rt_list_entry(node, struct rt_ai_core, list);
        if (rt_strncmp(object->name, name, RT_AI_NAME_MAX) == 0)
        {
            return object;
        }
    }
    return RT_AI_NULL;
}


/**
 * This function will initialize an object and add it to object system
 * management.
 *
 * @param object the specified object to be initialized.
 * @param type the object type.
 * @param name the object name. In system, the object's name must be unique.
 */
rt_ai_core_t rt_ai_core_register(struct rt_ai_core *object, enum rt_ai_obj_type type, const char *name)
{
    struct rt_ai_core_information *information;
    /* get object information */
    information = rt_ai_core_get_information(type);
    RT_AI_ASSERT_RETURN_NULL(information != RT_AI_NULL);
    /* try to find object */
    if(rt_ai_core_find(name,type)){
        return RT_AI_NULL;
    }
    
    object->type |= type;
    /* copy name */
    rt_strncpy(object->name, name, RT_AI_NAME_MAX);

    /* insert object into information object list */
    rt_list_insert_after(&(information->object_list), &(object->list));
    return object;
}

/**
 * This function will detach a static object from object system,
 * and the memory of static object is not freed.
 *
 * @param object the specified object to be detached.
 */
void rt_ai_core_detach(rt_ai_core_t object)
{
    /* object check */
    RT_AI_ASSERT_RETURN(object != RT_AI_NULL);
    /* remove from old list */
    rt_ai_list_remove(&(object->list));

}

void print_core_list(enum rt_ai_obj_type type){
    struct rt_ai_core_information *information = rt_ai_core_get_information(type);
    RT_AI_ASSERT_RETURN(information != RT_AI_NULL);
    struct rt_list_node *node = RT_AI_NULL;
    struct rt_ai_core *object = RT_AI_NULL;
    rt_list_for_each(node, &(information->object_list))
    {
        object = rt_list_entry(node, struct rt_ai_core, list);
        ai_log_raw("node name:%s,",object->name);
    }
    ai_log_raw("\n");
}

