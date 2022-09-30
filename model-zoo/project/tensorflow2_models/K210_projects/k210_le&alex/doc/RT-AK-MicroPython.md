# RT-AK MicroPython扩展模块

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



*关于RT-AK-MicroPython模块的代码实现和设计, 请参考`mpy_rt_ai` 文件下的相关文档.*