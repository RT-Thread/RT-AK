/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-28     liqiwen       the first version
 */

#include <rtthread.h>

#ifdef MICROPYTHON_USING_USEREXTMODS

#include <rt_ai.h>

#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "model_paser_helper.h"

#define __debug(_val) mp_printf(&mp_plat_print,"%s,%d," #_val "=%d\n",__FILE__,__LINE__,_val);

typedef struct
{
    mp_obj_base_t base;
    rt_ai_t handle;
    void *buf;
} py_model_obj_t;

#define py_model_cobj(py_obj) ((py_obj)->handle)

const mp_obj_type_t py_model_type;
STATIC mp_obj_t py_rt_ai_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args){
    enum
    {
        ARG_model,
        ARG_size,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_model, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        {MP_QSTR_size, MP_ARG_INT, {.u_int = 0}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    if(mp_obj_get_type(args[ARG_model].u_obj) != & py_model_type){
        mp_raise_TypeError("model type err!");
    }
    py_model_obj_t *model_obj = (py_model_obj_t *)args[ARG_model].u_obj;
    rt_ai_t handle = py_model_cobj(model_obj);
    uint32_t size = args[ARG_size].u_int;
    int stat = 0;
    if(!size){
        stat = rt_ai_init(handle, NULL);
    }
    else{
        model_obj->buf = m_malloc(size);
        stat = rt_ai_init(handle, model_obj->buf);
    }
    if(stat){
        mp_raise_OSError(stat);
    }
    if(!handle->info.input_n && !handle->info.input_n){
        handle->info.input_n = inputs_size(handle); __debug(handle->info.input_n);
        handle->info.output_n = outputs_size(handle); __debug(handle->info.output_n);

        for(int i=0; i<handle->info.input_n; i++){
                handle->info.input_n_stack[i] = inputs_n_bytes(handle, i); __debug(handle->info.input_n_stack[i]);
        }
        for(int i=0; i<handle->info.output_n; i++){
                handle->info.output_n_stack[i] = outputs_n_bytes(handle, i);__debug(handle->info.output_n_stack[i]);
        }
    }


    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_rt_ai_init_obj, 1, py_rt_ai_init);

static int ai_done(void *ctx)
{
    *((uint32_t*)ctx)= 1;
    return 0;
}
STATIC mp_obj_t py_rt_ai_run(mp_obj_t _obj, mp_obj_t data){

    if(mp_obj_get_type(_obj) != & py_model_type){
        mp_raise_TypeError("model type err!");
    }

    py_model_obj_t *model_obj = (py_model_obj_t*)_obj;
    volatile uint32_t g_ai_done_flag = 0;
    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(data, &bufinfo, MP_BUFFER_READ)){
        py_model_cobj(model_obj)->input[0] = bufinfo.buf;
        rt_ai_run(py_model_cobj(model_obj), ai_done, &g_ai_done_flag);
        while(!g_ai_done_flag);
    }
    else{
        mp_raise_ValueError("now only support image_t");
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(py_rt_ai_run_obj, py_rt_ai_run);

STATIC mp_obj_t py_rt_ai_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum
    {
        ARG_model,
        ARG_index,
        ARG_getlist,
    };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_model, MP_ARG_OBJ, {.u_obj = mp_const_none}},
        {MP_QSTR_index, MP_ARG_INT, {.u_int = 0}},
        {MP_QSTR_getlist, MP_ARG_BOOL, {.u_bool = 1}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if(mp_obj_get_type(args[ARG_model].u_obj) != & py_model_type){
        mp_raise_TypeError("model type err!");
    }
    py_model_obj_t *model_obj = (py_model_obj_t *)args[ARG_model].u_obj;
    rt_ai_t handle = py_model_cobj(model_obj);
    mp_obj_list_t *ret_list = NULL;    
    mp_int_t index = args[ARG_index].u_int;

    if((index < 0) || index > handle->info.output_n)
    {
        mp_raise_ValueError("excess of output");
    }
    float *out;
    out = rt_ai_output(handle, index);

    if(args[ARG_getlist].u_bool){
        ret_list = m_new(mp_obj_list_t, 1);
        mp_obj_list_init(ret_list, 0);

        for(int j = 0; j < (handle->info.output_n_stack[index])/sizeof(float); j++){
            mp_obj_list_append(ret_list, mp_obj_new_float(out[j]) );
        }

        return MP_OBJ_FROM_PTR(ret_list);
    }       
    return mp_const_none; 
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(py_rt_ai_output_obj, 1, py_rt_ai_output);

STATIC mp_obj_t make_new()
{
    py_model_obj_t *self = m_new_obj_with_finaliser(py_model_obj_t);

	self->base.type = &py_model_type;
    self->handle = RT_NULL;
    self->buf = NULL;
    return self;
}

STATIC const mp_rom_map_elem_t locals_dict_table[] = 
{
    {MP_ROM_QSTR(MP_QSTR_name), MP_OBJ_NEW_QSTR(MP_QSTR_name)},

};
STATIC MP_DEFINE_CONST_DICT(locals_dict, locals_dict_table);

const mp_obj_type_t py_model_type =
{
    {&mp_type_type},
    .name = MP_QSTR_model,
    // .make_new = make_new,
    .locals_dict = (mp_obj_t)&locals_dict
};

STATIC mp_obj_t py_rt_ai_find(mp_obj_t name){
    
    volatile uint32_t g_ai_done_flag = 0;
    if(mp_obj_get_type(name) == &mp_type_str){
        const char *model_name = mp_obj_str_get_str(name);
        rt_ai_t handle = rt_ai_find(model_name);
        if(!handle){
            mp_raise_ValueError("error! not find model");
        }
        py_model_obj_t *model_obj = m_new_obj(py_model_obj_t);
        model_obj->handle = handle;
        model_obj->base.type = &py_model_type;
        model_obj->buf = NULL;
        return MP_OBJ_FROM_PTR(model_obj);
    }
    else{
        mp_raise_TypeError("please type model name str!");
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(py_rt_ai_find_obj, py_rt_ai_find);

STATIC  mp_obj_t py_rt_ai_load(mp_obj_t buffer, mp_obj_t name){
    if(mp_obj_get_type(name) != &mp_type_str){
        mp_raise_TypeError("please type model name str!");
    }
    mp_buffer_info_t bufinfo;
    if (!mp_get_buffer(buffer, &bufinfo, MP_BUFFER_READ)){
        mp_raise_ValueError("get kmodel buffer error!");
    }

    const char *model_name = mp_obj_str_get_str(name);
    rt_ai_t handle = backend_k210_kpu_constructor_helper(bufinfo.buf, model_name);
    py_model_obj_t *model_obj = m_new_obj(py_model_obj_t);
    model_obj->base.type = &py_model_type;
    model_obj->handle = handle;
    model_obj->buf = RT_NULL;

    return MP_OBJ_FROM_PTR(model_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_2(py_rt_ai_load_obj, py_rt_ai_load);

STATIC mp_obj_t py_rt_ai_free(mp_obj_t _obj){
    if(mp_obj_get_type(_obj) != & py_model_type){
            mp_raise_TypeError("model type err!");
      }
    py_model_obj_t *model_obj = (py_model_obj_t*)_obj;
    backend_k210_kpu_kmodel_free(model_obj->handle);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(py_rt_ai_free_obj, py_rt_ai_free);

STATIC const mp_rom_map_elem_t rt_ak_module_globals_table[] = {
        { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_RT_AK) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_model), MP_ROM_PTR(&py_model_type)},
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_load), MP_ROM_PTR(&py_rt_ai_load_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_find), MP_ROM_PTR(&py_rt_ai_find_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_init), MP_ROM_PTR(&py_rt_ai_init_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_run), MP_ROM_PTR(&py_rt_ai_run_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_output), MP_ROM_PTR(&py_rt_ai_output_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_free), MP_ROM_PTR(&py_rt_ai_free_obj) },
};

STATIC MP_DEFINE_CONST_DICT(rt_ak_module_globals, rt_ak_module_globals_table);

const mp_obj_module_t rt_ak_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&rt_ak_module_globals,
};


#endif //PKG_USING_MICROPYTHON
