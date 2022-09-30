# 为 MicroPython 扩展 C 模块

当使用原生 MicroPython 进行开发时，你可能会遇到这样一些限制，比如官方没有实现自己想要的功能，或者你觉得这些实现不符合自己的工作需求。此时，添加自己的 C 模块到 MicroPython 中是一个不错的选择，你可以按照自己的想法，设计适合自己的 Python 函数调用。

为了帮助各位开发者快速添加 C 模块，RT-Thread 提供了相应的辅助工具 [C 绑定代码自动生成器](https://summerlife.github.io/RT-MicroPython-Generator/)。该工具可以帮助开发者自动生成 C 代码和 MicroPython 之间的接口层，开发者只需将 C 语言编写的功能代码添加到指定位置，MicroPython 即可直接调用该功能。

## Python 调用 C 函数的实现原理

C 语言和 Python 是两种完全不同的语言，如何使用 MicroPython 来调用 C 语言所实现的函数是许多小伙伴非常疑惑的地方。简单来说，这个问题的关键点在于，如何用 C 语言的形式在 MicroPython 源代码中**表述函数的入参和出参**。我举一个例子来讲解这个问题， 请观察如下 Python 函数：

```python
def add(a, b):
    return a + b
```

该函数有两个入参，返回一个参数。此时如果能用 C 语言表示该 **Python 函数的输入输出参数**，就可以将一个实现该功能的 C 函数对接到 MicroPython 中。

### 添加用户函数到 MicroPython

假设上述函数的参数类型都为整形，通过自动生成器可以得到如下样板函数：

```c
STATIC mp_obj_t add(
    mp_obj_t arg_1_obj,
    mp_obj_t arg_2_obj) {
    mp_int_t arg_1 = mp_obj_get_int(arg_1_obj);  /* 通过 Python 获取的第一个整形参数 arg_1 */
    mp_int_t arg_2 = mp_obj_get_int(arg_2_obj);  /* 通过 Python 获取的第二个整形参数 arg_2 */
    mp_int_t ret_val;

    /* Your code start! */

    ret_val = arg_1 + arg_2;  /* 处理入参 arg_1 和 arg_2，并将结果赋给返回值 ret_val */

    /* Your code end! */

    return mp_obj_new_int(ret_val);               /* 向 python 返回整形参数 ret_val */
}
MP_DEFINE_CONST_FUN_OBJ_2(add_obj, add);
```

生成器会处理好需要导出到 MicroPython 的函数的入参和出参，而开发者只需要编写相应的代码来处理这些输入参数，并且把返回值赋给输出参数即可。 通过包含头文件的方式，可以调用先前编写的 C 函数来对输入参数进行处理，或者根据输入参数来执行相应的动作，添加控制硬件的驱动的原理也是一样的。

最终使用 Python 调用 C 函数的效果如下：  

```python
>>> import userfunc
>>> userfunc.add(666,777)
1443
```

### 添加用户模块到 MicroPython

添加用户模块到 MicroPython 中也不难，首先应当熟练掌握上述添加 C 函数的过程，然后参考 PR [add module userfunc to MicroPython](https://github.com/RT-Thread-packages/micropython/pull/144) 来添加属于自己的模块，该 PR 实现了添加 `userfunc` 模块到 MicroPython 的功能，你可以按照同样的方式将自己编写的模块注册到 MicroPython 中，要注意仔细查看这个 PR 中修改的 4 个文件，不要漏掉修改的细节。
