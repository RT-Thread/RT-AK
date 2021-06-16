<center><h1>RT-AK 快速上手</h1></center>

[TOC]

*本项目为 `RT-AK` 的一个实战 `DEMO`。*

*致力于做一个保姆级教程*

*本教程 RT-AK 搭配平台插件：STM32*

- Windows 10
- Python >= 3.7

# 1. 准备工作

准备以下四份**重要重要重要**的材料：

| Index |     Prepare     | Example                              |
| :---: | :-------------: | ------------------------------------ |
|   1   | 硬件以及 `BSP`  | `ART-PI BSP`                         |
|   2   |  神经网络模型   | `./rt_ai_tools/Model/keras_mnist.h5` |
|   3   | `STM32 AI` 插件 | `X-CUBE-AI` 下载解压，下文有介绍     |
|   4   |     `RT-AK`     | `RT-AK` 代码克隆到本地               |

- 准备 `ART-PI BSP` ，以下二选一

  - RT-Thread Studio 创建 ART-PI [教程](https://art-pi.gitee.io/website/docs/#/development/ART-Pi_%E5%BC%80%E5%8F%91%E6%89%8B%E5%86%8C)
  - 我们准备好的下载地址： [ART-PI](http://117.143.63.254:9012/www/RT-AK/sdk-bsp-artpi.zip)

- 下载 `X-CUBE-AI` 

  - [下载地址](https://www.st.com/zh/embedded-software/x-cube-ai.html)

  - 版本: v5.2.0

## 1.1 X-CUBE-AI 介绍

`X-CUBE-AI` 是 `STM32Cube.AI` 生态系统的 `STM32Cube` 扩展软件包的一部分，能够自动转换预训练的神经网络。

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20201210165153.png)

<center><font size=2 color="gray">X-CUBE-AI</font></center>

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20201021163736.png)

<center><font size=2 color="gray">下载界面</font></center>

- [ ] **TO DO LIST**: 最新的版本是 `V6.0.0`，目前使用的是 `V5.2.0`，稍后的版本将会支持 `V6.0.0`

## 1.2 X-CUBE-AI 安装

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20201210165303.png)

<center><font size=2 color="gray">下载后的文件夹界面</font></center>

其中：

- `stm32ai-windows-5.2.0.zip` 是我们所需要，该文件夹里面存放的是 `X-CUBE-AI` 模型转换软件： `stm32ai` 
- `STMxxx.pack` 是 `STM32Cube.AI` 的静态库文件，**无需解压，已经存在**，位于 `./rt_ai_tools/platforms/stm32/X-CUBE-AI.5.2.0`

解压 `stm32ai-windows-5.2.0.zip` 。

在这里我将其解压到：`D:\Program Files (x86)\stm32ai-windows-5.2.0` ，可以在该路径下看见有一个 `windows` 文件夹。

**记下该路径，重要重要重要**

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401162400.png)

<center><font size=2 color="gray">STM32: X-CUBE-AI 解压路径</font></center>

# 2. 执行步骤

> 代码将会自动使用 `STM32Cube.AI` 的模型转换工具，获得一个集成了 AI 的 BSP
>
> 对，就是这么硬核，一步肝到位！

内部的流程请看源码或者 `plugin_stm32` 仓库下的 `readme` 文档

## 2.1 基础运行命令

请在 `edge-ai/RTAK/tools` 路径下运行该程序。

![image-20210412194422222](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210412194423.png)

```shell
# 基础运行命令
python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path> --clear

# 示例
python aitools.py --project="D:\RT-ThreadStudio\workspace\test" --model="./Models/keras_mnist.h5" --platform=stm32 --ext_tools="D:\Program Files (x86)\stm32ai-windows-5.2.0\windows" --clear
```

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210414174057.png)

## 2.2 其他运行参数补充说明

```shell
# 指定转换模型的名称，--model_name 默认为 network
python aitools.py --project=<your_project_path> --model=<your_model_path>  --model_name=<model_name>  --platform=stm32 --ext_tools=<your_x-cube-ai_path>

# 保存运行 stm32ai 线程过程中产生的文件，--clear 默认为空
# 如果存在，则将会删除`stm32ai` 运行时产生的工作文件夹，即`--stm_out`
python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path>

# 指定保存运行日志, --log 默认为空
python aitools.py --project=<your_project_path> --model=<your_model_path> --log=./log.log --platform=stm32 --ext_tools=<your_x-cube-ai_path>

# 指定保存的文件夹名称，--stm_out 默认是当天时间，比如 './20210223'
python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path> --stm_out <new_dir>

# 指定生成的 c-model 名，--c_model_name 默认是network
python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path> --c_model_name=<new_model_name>
```

## 2.3 运行参数详细说明

- 主函数参数部分

| Parameter             | Description                                                  |
| --------------------- | ------------------------------------------------------------ |
| `--log`               | `log` 日志存放路径，默认是空，如果有，则将保存log日志文件，例如：`./log.log` |
| `--project`           | `OS+BSP` 项目工程文件夹，默认为空，**需要用户指定**          |
| `--model`             | 神经网络模型文件路径，默认为 `./Models/keras_mnist.h5`       |
| `--model_name`        | 神经网络模型转换后新的模型名，默认是 `network`               |
| `--rt_ai_lib`         | `RT-Thread` 官方提供的 `RT-AK Lib` ，默认是 `../rt_ai_lib`   |
| `--support_platforms` | 存放 `RT-AK` 支持的第三方目标平台插件，`platform: platform_git_url` 格式           |
| `--platform`          | 指定目标平台信息，目前支持：`stm32`、`k210`，默认是 `example`，具体体可用的目标平台由 `platforms/xxx.json` 注册 |

- STM32 平台插件参数部分

| Parameter           | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| **`--ext_tools`**   | **`X-CUBE-AI` 存放路径，模型转换工具，内有 `stm32ai` 可执行软件，需要用户指定** |
| `--cube_ai`         | `X-CUBE-AI` 运行所需的静态库，默认为`./platforms/stm32/X-CUBE-AI.5.2.0` |
| `--rt_ai_example`   | 存放`rt_ai_<model_name>_model.c` 示例文件，默认是 `./platforms/stm32/docs` |
| `--stm_out`         | 经过 `stm32ai` 线程处理之后产生的中间文件夹路径，默认是当天的时间戳命名 |
| `--workspace`       | `stm32ai` 运行时产生的临时工作区，默认是`./stm32ai_ws`       |
| `--val_data`        | 默认为空，即使用内部自生成的随机数据集，允许用户自定义测试数据集， |
| `--compress`        | 表示将应用的全局压缩因子，仅应用在全连接层，可选 "1\|4\|8"，默认值：`1` |
| `--batches`         | 指示生成了多少随机数据样本，默认是`10`                       |
| `--mode`            | "analyze\|validate" 模式（可选）+”generate“模式（必须有），`1`表示选中，在`{'001', '011', '101', '111'}`中选一个，默认是 `001` |
| **--network**       | **在 `Documents` 中的模板文件的模型名，默认是 `mnist`**      |
| **--enable_rt_lib** | **在 `project/rtconfgi.h` 中打开宏定义，默认是 `RT_AI_USE_CUBE`** |
| --clear              | 是否需要删除 `stm32ai` 生成的中间文件夹 `stm_out` ，默认为`False` |

# 3.编译

以下任意选择一个即可

- `Keil`
- `RT-Thread Studio`
- 基于 `RT-Thread Env` 的 `Scons`
- `Scons`

## 3.1 MDK Keil 编译

- [Env 工具](https://www.rt-thread.org/document/site/#)

进入到 `BSP` 项目工程路径，右键打开 Env。

执行：

```shell
$ scons --target=mdk5
```

烧录的时候可能会遇到一些烧录算法问题，解决方案👉[传送门](https://gitee.com/RT-Thread-Studio-Mirror/sdk-bsp-stm32h750-realthread-artpi/blob/master/documents/UM5005-RT-Thread%20ART-Pi%20%E5%B8%B8%E8%A7%81%E9%97%AE%E9%A2%98%E8%A7%A3%E7%AD%94.md)

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401192909.png)

## 3.2 RT-Thread Studio 编译

> 运行该章节的前提是：
>
> 你的 ART-Pi BSP 通过 RT-Thread Studio 生成

在 `RT-Thread Studio` 中，找到项目工程，右键，

**更新软件包 + 刷新工程**，编译烧录即可

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401193458.png)

## 3.3 Env Scons 编译

- [Env 工具](https://www.rt-thread.org/document/site/#)

进入到 `BSP` 项目工程路径，右键打开 Env

执行：

```shell
# 这是个好习惯
$ scons -c
$ scons -j 6
```

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401182746.png)

## 3.4 Scons 独自编译

1. 获取编译工具链

   我们已经准备好一份编译工具链，下载安装即可👉 [mingw](http://117.143.63.254:9012/www/RT-AK/mingw.zip)

2. 设置系统环境变量

   在 `cmd` 中运行以下命令

   **注意在该小节当中 `cmd` 始终为同一个**

   ```shell
   # 解压路径注意了，最好不要中文不要空格，就正常英文来就好
   $ set RTT_EXEC_PATH=D:\Project\tmp\mingw\bin
   
   # 查看是否设置成功
   $ set
   ```

   ![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401183956.png)

3. 安装 `scons`

   ```shell
   $ pip3 install scons
   ```

4. 编译

    ```shell
    $ cd <project_path>

    # 这是个好习惯
    $ scons -c
    $ scons -j 6
    ```

5. 成果展示

   ```shell
   D:\RT-ThreadStudio\workspace\test>scons -c
   scons: Reading SConscript files ...
   RTT_EXEC_PATH:  D:\Project\tmp\mingw\bin
   D:\RT-ThreadStudio\workspace\test
   ...
   LINK rt-thread.elf
   arm-none-eabi-objcopy -O binary rt-thread.elf rtthread.bin
   arm-none-eabi-size rt-thread.elf
      text    data     bss     dec     hex filename
    154096    4480    4484  163060   27cf4 rt-thread.elf
   scons: done building targets.
   ```

---

- 如果之前有安装过 `utils-core`，需要先卸载 `pip uninstall utils-core`
- 如果想要继续使用 `RT-Thread Env` 开发的，需要先删除 `<project>/.sconsign.dblite`

# 4. 烧录

- [x] `MDK Keil`

- [x] `RT-Thread Studio`

- [x] `STM32Programmer-cli`

  命令行烧录工具：`STM32Programmer`

  位于 `<edge-ai_path>/RTAK/rt_ai_tools/platforms/stm32/stm32programmer_cli`

  ```shell
  $ cd <edge-ai_path>/RTAK/rt_ai_tools/platforms/stm32/stm32programmer_cli/bin
  
  # download
  $ STM32_Programmer_CLI.exe -c port=SWD --extload ExternalLoader/ART-Pi_W25Q64.stldr -d <BSP_path>/rtthread.bin 0x90000000 -hardRst -s 
  ```

  ![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401192319.png)

这时候你就已经成功获得了一个集成了 `AI` 和 `RT-Thread` 的新的 `ART-Pi BSP`，

就可以`RT-Thread` 系统上做应用开发啦。

## 示例应用代码提供

我们提供了一份运行模型推理的示例应用代码，可参考 [5. RT-AK MNIST 应用参考示例](# 5. RT-AK MNIST 应用参考示例)，也可直接下载。

- [mnist_app.c](http://117.143.63.254:9012/www/RT-AK/mnist_app.zip)：

1. 下载解压，放置到 `<BSP>/applications` 路径下

![image-20210409192729527](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210409192730.png)

2. 选中 RT-Thread Studio 中的 项目工程，右击刷新

![image-20210409193019579](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210409193022.png)

3. 编译烧录，
4. 输入命令：`mnsit_app`

![image-20210409192638719](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210409192639.png)

---

完整的示例工程：https://github.com/EdgeAIWithRTT/Project3-Mnist_Cube_RTT/tree/master/Mnist_RTT

# 5. RT-AK MNIST 应用参考示例

```c
#include <rt_ai_mnist_model.h>
#include <rt_ai.h>
#include <rt_ai_log.h>
/* 分配输入输出以及运行时内存 */
static rt_ai_buffer_t work_buffer[RT_AI_MNIST_WORK_BUFFER_BYTES+RT_AI_MNIST_IN_TOTAL_SIZE_BYTES+RT_AI_MNIST_OUT_TOTAL_SIZE_BYTES];
/* 定义模型句柄 */
static rt_ai_t model = NULL;
/* mnist手写数字数据 */
static const float input_data0[] = MNIST_0_7;
static const float input_data1[] = MNIST_1_2;

int ai_run_complete_flag = 0;
void ai_run_complete(void *arg){
    *(int*)arg = 1;
}

int mnist_app(void){

    int result = 0;
    int prediction = 0;
    //查找已注册的模型句柄
    model = rt_ai_find(RT_AI_MNIST_MODEL_NAME);
    if(model == RT_AI_NULL){
        return -1;
    }
    //初始化模型
    result = rt_ai_init(model , work_buffer);
    if(result != 0){
        return -1;
    }
    //准备数据
    rt_memcpy(model->input[0],input_data0,RT_AI_MNIST_IN_1_SIZE_BYTES);
    result = rt_ai_run(model , ai_run_complete, &ai_run_complete_flag);

    //处理推理结果
    if(ai_run_complete_flag){
        //获取输出结果
        float *out = (float *)rt_ai_output(model,0);
        //判断概率最大索引
        for(int i = 0 ; i < 10 ; i++){
            if(out[i] > out[prediction]){
                prediction = i;
            }
        }
        AI_LOG("The Mnist prediction is : %d\n", prediction);
    }
    return 0;
}
MSH_CMD_EXPORT(mnist_app,mnist demo);
```

应用开发 `API` 见 [rt_ai_lib/readme](../rt_ai_lib)

