# RT-AK MicroPython使用说明

## 简介

**RT-AK** 为RT-Thread下提供的一套端侧AI运行框架. 能够对接多种端侧的神经网络推理框架,  提供统一的应用层接口.  该文档为针对[RT-AK](https://github.com/RT-Thread/RT-AK)的MicroPython使用和适配说明. 建议用户可先对C语言接口使用方法有所了解.

- 目前主要针对搭载k210芯片的Draco开发板完成适配. 

## 编译配置说明

要是用RT-AK的MicroPython接口, 需要在RT-Thread Studio的Draco开发板工程的RT-Thread Settings中进行配置. 并且需要RT-AK检查工程根路径下是否有`rt_ai_lib`文件夹(C语言库). RT-Thread Settings的配置方式如下: `RT-Thread Settings -> mpy-extmods -> Enable MPY extmods -> Enable RT-AK MPY module` 

*k210本身具有片上6M的通用内存, MicroPython运行时会预先从系统中分配一段内存用作MicroPython运行时内存. 对于开启并使用RT-AK MicroPython模块时, 由于k210 sdk运行AI时中会调用`malloc`从系统中分配一定的内存用于AI计算, 因此建议对于MicroPython运行时分配的堆内存不要太大, 以免导致AI初始化失败. 对于MicroPython软件包中的配置选项`Heap size for python run environment` 根据经验可配置为`[600000-1500000]` 之间.*

## RT-AK MicroPython函数使用说明

### rt_ak模块

RT-AK的所有python函数包含在`rt_ak`模块中, 使用时应`import rt_ak`. 在python环境下使用`dir(rt_ak)`语句可查看模块所包含的函数名词.

#### Model对象

神经网络模型对象, 保存注册到RT-AK中的模型信息, 以及运行环境等.

#### ai_find(name)

**描述:** 该函数通过一个字符串类型的名字, 用于查找一个已注册到RT-AK中的模型. 若查找成功将返回一个`Model`对象实例, 否则返回`None`

**参数:**

- `name` 为要查找的模型名, 模型为通过RT-AK工具预转换注入工程的模型.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
```

**备注:** 该方法对应RT-AK C接口`rt_ai_find()`

#### ai_load(buffer, name)

**描述:** 该方法与`ai_find` 类似, 用于从文件系统中加载模型,  需要首先从文件系统中把模型文件read到内存buffer中, 调用该方法加载buffer.

**参数:**

- `buffer` 内存中模型buffer
- `name` 传入该模型的模型名

**示例:**

```python
f=open('yolov3.kmodel')
yv3_kmodel=f.read()
yolo=rt_ak.ai_load(yv3_kmodel, 'yolov3')
rt_ak.ai_init(yolo)
```

**备注:** `ai_find` 方法用于查找已编译存在固件中的模型. `ai_load` 用于动态的从文件系统中加载模型. 通常更具情况使用其中一个方法.

#### ai_init(model, size=0)

**描述:** 该函数初始化一个模型对象. `model`参数必须传入, `size`为运行时需要的内存, 在K210上指示模型输入的buffer大小. 可选参数. 无返回值.

**参数:** 

- `model` 通过`ai_find` 或 `ai_load` 加载的模型对象.
- `size` 可选参数, 用于预分配内存.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
rt_ak.ai_init(model)
```

**备注:** 该函数对应RT-AK C接口`rt_ai_init()`.

#### ai_run(model, input)

**描述:** 该函数执行模型一次前向推理, 参数`model`为要进行推理的模型, `input`为模型的输入数据, 无返回值.

**参数:** 

- `model` 通过`ai_find` 或 `ai_load` 加载的模型对象.
- `input` 模型的输入.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
rt_ak.ai_init(model)
rt_ak.ai_run(model,img_888_chw)
```

**备注:** 该函数对于RT-AK C接口`rt_ai_run()`.

#### ai_output(model, index=0)

**描述:** 该函数返回最近一次执行模型推理的输出结果. 参数`model`为模型, `index`为要获取的输出索引. 返回值为结果的`list`对象.

**参数:** 

- `model` 通过`ai_find` 或 `ai_load` 加载的模型对象.
- `index` 模型输出索引.

**示例:**

```python
model=rt_ak.ai_find("cifar10_mb")
rt_ak.ai_init(model)
rt_ak.ai_run(model,img_888_chw)
predicts=rt_ak.ai_output(model,0)
predicts

# out[]: [0.0, 0.9, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0]
```

**备注:** 该函数对于RT-AK C接口`rt_ai_output()`.

#### ai_free(model)

**描述:** 用于释放模型相关内存, 在模型不再使用时, 应调用此函数对模型进行内存释放.

**参数:**

- `model` 要被释放的模型.

# RT-AK模块设计说明

## 简介

该文档用于简要说明RT-AK MicroPython扩展模块的设计及实现.

关于MicroPython添加自定义模块及相关接口说明给出如下相关参考资料: 

- [MicroPython – 电子创客营 (eemaker.com)](https://www.eemaker.com/code/micropython)

- [micropython-usermod](https://micropython-usermod.readthedocs.io/en/latest/usermods_01.html)
- [MicroPython C 模块扩展 (rt-thread.org)](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/micropython-docs/external_c_modules)

## C=>MicroPython开发基础

### Python相关概念

熟悉python的小伙伴会知道, python是面向对象的. 了解的更深一点的人或许知道, 与C语言不同, python中的变量本质上是个**引用**, 类似C语言的**指针**. 而指针可以理解为是一种**弱类型**的变量, 其指向一个内存地址. 因此在python中，**类型属于对象**，对象有不同类型的区分，变量是没有类型的, 对象是在内存中的一个实体, 而**变量是对其的一个引用(指针)**. 带着这个概念, 来理解用C开发Pyhthon模块会更加清晰.

一个基础的python函数其底层C函数的基本形式如下:

```c
mp_obj_t some_function(mp_obj_t some_variable, ...) {
    // some_variable is converted to fundamental C types (bytes, ints, floats, pointers, structures, etc.)
    ...
}

// 其中 typedef (void *) mp_obj_t 
```

某个类型的变量被传递给函数，函数将结果作为类型的对象返回。基本上，该变量只不过是一个8字节(地址位宽)的内存段，所有具体对象都需要进一步通过该变量进行进步解析获得。对于Python对象到C层面的解析, 有一系列宏可做判断.

如果我们想知道该变量是否是一个整数，我们可以检查布尔值

```c
MP_OBJ_IS_SMALL_INT(some_variable)
```

存储在其中的整数值可以通过调用下面宏获得:

```c
int value_of_some_variable = MP_OBJ_SMALL_INT_VALUE(some_variable);
```

在C层面, 想要返回一个整数给python, 同样需要C-->Python的编码:

```c
return mp_obj_new_int(value_of_some_variable);
```

更泛型的类型可以用宏处理，宏将对象作为第一个参数，并将指向该类型的指针作为第二个参数。现在，如果你想知道，是否是一个元组，你可以应用宏:

```c
mp_obj_is_type(some_variable, &mp_type_tuple);
```

判断变量所指的对象:

```c
mp_obj_is_type(myobject, &my_type);
```

还有关于用C添加MicroPython模块以及对象的相关方法参考[micropython-usermod](https://micropython-usermod.readthedocs.io/en/latest/usermods_01.html)

### RT-AK对接MicroPython

**源文件列表:**

| 源文件                 | 说明                                                         |
| ---------------------- | ------------------------------------------------------------ |
| rt_ai_mpython.c/.h     | 该文件为`rt_ak`模块及相关函数主要适配代码                    |
| moddefs_rt_ai.h        | 该头文件为模块声明文件, 该头文件被`moddefs.user.extmods.h`包含, 用于导入MicroPython |
| qstrdefs_rt_ai.h       | 标识字符串, 如python中所输入的如变量名、函数名等用字符串进行标识 |
| model_paser_helper.c/h | 模型解析相关的辅助函数.                                      |

RT-AK本身面向嵌入式平台, 使用C语言开发, 在此基础上扩展了MicroPython作为其开发语言. 本节简要描述RT-AK对接MicroPython的基本框架.

RT-AK本身有如下C语言接口:

```c
rt_err_t rt_ai_init(rt_ai_t ai, rt_ai_buffer_t* work_buf);
rt_err_t rt_ai_run(rt_ai_t ai, void (*callback)(void * arg), void *arg);
rt_ai_t  rt_ai_find(const char *name);
rt_ai_buffer_t* rt_ai_output(rt_ai_t ai, rt_uint32_t index);
```

在MicroPython层面使用`rt_ak.ai_find()` `rt_ak.ai_init()` 等接口进行调用.

MicroPython解析器提供了一套接口, 用于将C语言函数和变量对接到MicroPython层面. 在使用Python调用函数时, 最终会调用到C语言函数. 因此对RT-AK的一套函数使用MicroPython接口对其进行包装. 以`rt_ai_find`为例:

```c
STATIC mp_obj_t py_rt_ai_find(mp_obj_t name){
    volatile uint32_t g_ai_done_flag = 0;
    if(mp_obj_get_type(name) == &mp_type_str){ // 判断python层面传入的参数类型
        const char *model_name = mp_obj_str_get_str(name); // 获取字符串(模型名)
        rt_ai_t handle = rt_ai_find(model_name); // 注意这里调用了RT-AK的C语言接口
        if(!handle){
            mp_raise_ValueError("error! not find model");
        }
        py_model_obj_t *model_obj = m_new_obj(py_model_obj_t); //创建一个新的对象
        model_obj->handle = handle;	// 将rt_ai_t类型包装到python对象结构体中
        model_obj->base.type = &py_model_type; // 指定对象类型
        model_obj->buf = NULL;	// 预分配内存
        return MP_OBJ_FROM_PTR(model_obj); // 返回python对象
    }
    else{
        mp_raise_TypeError("please type model name str!");
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(py_rt_ai_find_obj, py_rt_ai_find); // 使用MicroPython接口包装函数.
```

上述代码展示了C函数 `rt_ai_find()` 对接到python接口`rt_ak.ai_find()` 函数的实现. 

再来了解一下`rt_ak.ai_load()` 方法, 适配代码如下:

```c
STATIC  mp_obj_t py_rt_ai_load(mp_obj_t buffer, mp_obj_t name){
    if(mp_obj_get_type(name) != &mp_type_str){		// 判断python传入参数的类型
        mp_raise_TypeError("please type model name str!");
    }
    mp_buffer_info_t bufinfo;	// 定义一个mpy buffer信息结构体
    // 从参数`buffer`中获取buffer的真正地址.
    if (!mp_get_buffer(buffer, &bufinfo, MP_BUFFER_READ)){ 
        mp_raise_ValueError("get kmodel buffer error!");
    }
	// 获取字符串
    const char *model_name = mp_obj_str_get_str(name);
    //模型输入输出信息解析, 模型解析辅助函数定义在model_paser_helper中.
    rt_ai_t handle = backend_k210_kpu_constructor_helper(bufinfo.buf, model_name);
    py_model_obj_t *model_obj = m_new_obj(py_model_obj_t); //定义一个新的对象(分配内存).
    model_obj->base.type = &py_model_type; // 初始化对象类型.
    model_obj->handle = handle;	// 将rt_ai_t包装在python对象结构体中.
    model_obj->buf = RT_NULL; 

    return MP_OBJ_FROM_PTR(model_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_2(py_rt_ai_load_obj, py_rt_ai_load); // MicroPython接口包装函数
```

上述代码为`rt_ak.ai_load()` 方法的代码. 该函数用于从buffer(内存)中读取模型文件, 并动态`new` 一个python对象. 之后借助模型信息解析辅助函数`backend_k210_kpu_constructor_helper`来解析模型信息, 构造RT-AK结构体. 

**补充:** 在 `rt_ak_ai_load()` 中调用了模型信息解析的辅助函数`backend_k210_kpu_constructor_helper`. 该函数解析了K210官方的`kmodel`模型. 为实现动态加载kmodel功能, 在Draco开发板工程中的`kendryt-sdk`中修改了官方sdk代码. 具体如下, 在`nncase.cpp`源文件中的`class nncase_context`类中添加了4个友元函数, 用于获取私有类成员信息, 对于友元函数的具体实现较为简单, 仅仅用于获取值并返回, 具体实现请参考`nncase.cpp` 源文件中代码.

```c
class nncase_context
{
public:
    friend size_t km_inputs_size(kpu_model_context_t *ctx); //获取kmodel输入数量
    friend size_t km_outputs_size(kpu_model_context_t *ctx); //获取kmodel输出数量
    friend size_t km_inputs_n_bytes(kpu_model_context_t *ctx, size_t index); //获取输入字节数
    friend size_t km_outputs_n_bytes(kpu_model_context_t *ctx, size_t index); //获取输出字节数
	...
private:
    interpreter_t interpreter_;
    kpu_done_callback_t done_callback_;
    void *userdata_;
}
```

上述便为RT-AK的C函数对接MicroPython函数的示例. 在python层面RT-AK所有的函数都属于`rt_ak` 模块.

### rt_ak Module

python模块是一组类、函数或变量的集合, 在python语言中通常使用`import module` 来导入一个模块. 本小结描述`rt_ak` 模块的实现. C层面使用键值对的方式来描述python变量. 键为python层显示的变量名, 值为变量的值. 

```c
STATIC const mp_rom_map_elem_t rt_ak_module_globals_table[] = {
        // 键 对应python层面rt_ak.__name__(变量)            值(字符串)
        { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_RT_AK) },
        // 键 rt_ak.model(变量)                    值(对象)
        { MP_OBJ_NEW_QSTR(MP_QSTR_model), MP_ROM_PTR(&py_model_type)},
        // 键 对应rt_ak.ai_find()                      值(C函数实现)
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_find), MP_ROM_PTR(&py_rt_ai_find_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_init), MP_ROM_PTR(&py_rt_ai_init_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_run), MP_ROM_PTR(&py_rt_ai_run_obj) },
        { MP_OBJ_NEW_QSTR(MP_QSTR_ai_output), MP_ROM_PTR(&py_rt_ai_output_obj) },
    	{ MP_OBJ_NEW_QSTR(MP_QSTR_ai_free), MP_ROM_PTR(&py_rt_ai_free_obj) },
};
    // 理解为rt_ak_module_globals 注册到rt_ak_module_globals_table
STATIC MP_DEFINE_CONST_DICT(rt_ak_module_globals, rt_ak_module_globals_table);
// 定义一个模块
const mp_obj_module_t rt_ak_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&rt_ak_module_globals,
};
```

关于MP_QSTR_xxx,  MicroPython使用字符串池的方式来保存RAM和ROM内容。这避免了必须存储相同字符串的重复副本。这主要适用于代码中的标识符，因为像函数或变量名这样的东西很可能出现在代码中的多个位置。在MicroPython中，嵌入字符串称为QSTR(唯一字符串)。例如在python中输入函数, 并且使用关键字参数时如`add(a=1, b=2)`其中`add` `a` `b`都是作为字符串输入到python解析器中, 在C层面中使用`MP_QSTR_add` `MP_QSTR_a` `MP_QSTR_a`来进行匹配函数名和变量名, 使用字符串标识的方式避免了在C层面多处使用`"xxxx"`的文字池.

QSTR值(类型为QSTR)是QSTR池的索引。qstr存储它们的长度和内容的哈希值，以便在重复数据删除过程中进行快速比较。所有与字符串相关的字节码操作都使用QSTR参数. 例如在MicroPython C代码中，任何应该被嵌入到最终固件中的字符串都被写成MP_QSTR_Foo。在编译时，它将求值为qstr值，该值指向qstr池中“Foo”的索引.

QSTR值可使用工程跟目录下的genqstr.py来进行生成.

### Model

在C语言层面, RT-AK使用`rt_ai_t` 作为模型结构体. 其在MicroPython层面被包装为`model` 对象. 其对象结构体如下:

```c
// 类型对应的C实现
typedef struct
{
    mp_obj_base_t base; //每个python对应的c类型的具体实现需要包含该成员, 该成员指向const mp_obj_type_t py_model_type;
    rt_ai_t handle;
    void *buf;
} py_model_obj_t;
// 类型标识
const mp_obj_type_t py_model_type =
{
    {&mp_type_type},
    .name = MP_QSTR_model,
    // .make_new = make_new,
    .locals_dict = (mp_obj_t)&locals_dict
};
```

### 

|      |      |
| ---- | ---- |
|      |      |
|      |      |
|      |      |
|      |      |
