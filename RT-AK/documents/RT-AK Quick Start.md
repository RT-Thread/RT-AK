# OpenSource AI Kit: One Click to Deploy AI Models on RT-Thread

# 0. RT-AK Intro

`RT-AK` is an `AI` Kit developed by the RT-Thread Team, committed to help developers one-click deploy the `AI` models, allowing developers to develop business code on top of a unified API and ultimately optimize performance on the target platform, making it easier and simple to develop end-side `AI` applications.

With the `RT-AK` support, taking one line of command of `python aitools.py --model xxx...` can deploy the `AI` model to the `RT-Thread` embedded system:

Commands List: [How to run aitools.py](# step1 Run `rt_ai_tools/aitools.py`)
GitHub Repository: https://github.com/RT-Thread/RT-AK

---

We are striving to minimize the difficulties and thresholds for embedded AI deployment!

# 1. Preparation

- Windows 10
- Python >= 3.7

Required Documents:

| Index |      Documents       | Example                                                    |
| :---: | :------------------: | ---------------------------------------------------------- |
|   1   |   Hardware &`BSP`    | `ART-PI BSP`                                               |
|   2   | Neural network model | `./rt_ai_tools/Model/keras_mnist.h5`                       |
|   3   |  `STM32 AI` Plug-in  | `X-CUBE-AI` download & decompress, will be described below |
|   4   |       `RT-AK`        | `RT-AK` code is cloned locally                             |

## 1.1 X-CUBE-AI Intro

- Download Link: https://www.st.com/zh/embedded-software/x-cube-ai.html 

- Version:5.2.0

  > **TO DO LIST**: The Latest comes to V6.0.0, RT-AK is now using V5.2.0 will soon be updated to the latest!

`X-CUBE-AI` is a part of `STM32Cube` Expansion Package in `STM32Cube.AI` ecosystem and extending STM32CubeMX capabilities with automatic conversion of pre-trained Neural Network and integration of generated optimized library into the user's project. 

![image-20210525153037964](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210525153039.png)

<center><font size=2 color="gray">X-CUBE-AI</font></center>
![image-20210525153057003](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210525153058.png)

<center><font size=2 color="gray">下载界面</font></center>
## 1.2 X-CUBE-AI Decompress

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20201210165303.png)

<center><font size=2 color="gray">Downloaded File Folder</font></center>
Includes: 

- `stm32ai-windows-5.2.0.zip` has stored the `X-CUBE-AI` model conversion software: `stm32ai`. This file is required. 
- `STMxxx.pack` is a static library file of  `STM32Cube.AI` , it doesn't need to decompress, and located in `./RT-AK/rt_ai_tools/platforms/stm32/X-CUBE-AI.5.2.0`

Unzip `stm32ai-windows-5.2.0.zip` file

to`D:\Program Files (x86)\stm32ai-windows-5.2.0` , so you can get a  `windows` folder under this path.

Keep in mind about this path. HIGH ATTENTION!

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210401162400.png)

<center><font size=2 color="gray">STM32: X-CUBE-AI 解压路径</font></center>
## 1.3 ART-PI BSP

1.  Open [RT-Thread Studio](https://www.rt-thread.io/studio.html)

2.  Create `ART-PI BSP` 

<img src="https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210524181008.png" style="zoom:80%;" />

<center><font size=2 color="gray">Create ART-PI Project in RT-Thread Studio</font></center>
# 2. Execute

## Step1 Run `rt_ai_tools/aitools.py`

> Code will automatically use the `STM32Cube.AI` model conversion tool to obtain a BSP with AI integrated. YES. Aha Moment!

Internal detailed processes please refer to the source code or the `readme` documentation under the `plugin_stm32` warehouse.

### Run Command

Enter `edge-ai/RTAK/tools` path, run `aitools.py`

![image-20210412194422222](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210412194423.png)

```shell
# Run Command
python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path> --clear

# Sample
python aitools.py --project="D:\RT-ThreadStudio\workspace\test" --model="./Models/keras_mnist.h5" --platform=stm32 --ext_tools="D:\Program Files (x86)\stm32ai-windows-5.2.0\windows" --clear
```


Works done so far, some more additional instructions to supplement are shown following

![image-20210414174056329](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210414174057.png)

### Other Parameters for Run Command 

```shell
# Specify the name of the conversion model,--model_name defaults to network
python aitools.py --project=<your_project_path> --model=<your_model_path>  --model_name=<model_name>  --platform=stm32 --ext_tools=<your_x-cube-ai_path>

# Save files generated during running the stm32ai thread,-- clear defaults to empty

# If exited, the work folder generated during running 'stm32ai', i.e. '--stm_out, will be deleted

python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path>

# Specify to save the run log, --log defaults to empty

python aitools.py --project=<your_project_path> --model=<your_model_path> --log=./log.log --platform=stm32 --ext_tools=<your_x-cube-ai_path>

# Specify the name of the saved folder,--stm_out defaults to the time of day, such as './20210223'

python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path> --stm_out <new_dir>

# Specify the generated c-model name ,--c_model_name defaults to network

python aitools.py --project=<your_project_path> --model=<your_model_path> --platform=stm32 --ext_tools=<your_x-cube-ai_path> --c_model_name=<new_model_name>
```

### Run Parameters Specs

- Main functions parameters

| Parameter      | Description                                                  |
| -------------- | ------------------------------------------------------------ |
| `--log`        | `./log.log` The `log`  stores the path, which defaults to empty, and if so, the log file is saved, e.g. `./log.log` |
| `--project`    | `OS+BSP` project engineering folder, empty, user-specified   |
| `--model`      | Neural network model file path, defaults to `./Models/keras_mnist.h5` |
| `--model_name` | New model name after neural network model conversion, defaults to `network` |
| `--rt_ai_lib`  | `RT-AK Lib` provided by `RT-Thread` defaults to `../rt_ai_lib` |
| `--platform`   | Specified hardware platform information, currently supported: `stm32`, `k210`, defaults to `example` |

- STM32 platform plug-in parameters

| Parameter           | Description                                                  |
| ------------------- | ------------------------------------------------------------ |
| **`--ext_tools`**   | `X-CUBE-AI` storage path, model conversion tool, `stm32ai` executable software inside, requiring user-specified |
| `--cube_ai`         | `X-CUBE-AI` runs the required static library, defaults to`./platforms/stm32/X-CUBE-AI.5.2.0` |
| `--rt_ai_example`   | Store`rt_ai_<model_name>_model.c` sample file, defaults to`./platforms/stm32/docs` |
| `--stm_out`         | The intermediate folder path that results from the `stm32ai` thread processing is named by default for the timestamp of the day |
| `--workspace`       | Temporary workspace generated during running `stm32ai`, defaults to `./stm32ai_ws` |
| `--val_data`        | Defaults to empty, allows users to customize the test dataset even when using an internally self-generated random dataset. |
| `--compress`        | Represents the global compressibility factor that will be applied, applied only to the full connection layer, with the option of "1\|4\|8", defaults to `1` |
| `--batches`         | Indicates how many random data samples have been generated, with the default of `10` |
| `--mode`            | "analyze\|validate" mode (optional)  plus "generate" mode (must have), `1` indicates selected, select one from  `{'001', '011', '101', '111'}`, defaults to `001` |
| **--network**       | The model name of the template file in `Documents`, defaults to `mnist`. |
| **--enable_rt_lib** | Open macro definitions in `project/rtconfgi.h` and default to `RT_AI_USE_CUBE` |
| --clear             | If need to delete the intermediate folder `stm_out` that generated in `stm32ai`, defaults to `False` |

## Step2 Compile and Burn

This tutorial shows only 'RT-Thread Studio' compilation method, but RT-AK also supports:

- `Keil`
- `Scons` based on `RT-Thread Env`
- `Scons` not based on `RT-Thread Env`

### RT-Thread Studio Compile and Burn

In `RT-Thread Studio`, find the project engineering and right-click

Update packages and refresh project,

Then compile（![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210409183341.png)）

Finally, burn and display.

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210524182418.png)

![编译成功](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210524182508.png)

![](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210524182623.png)

Now, you're successfully get a new `ART-Pi BSP` that integrates `AI` and `RT-Thread`.

AI-related application development can be easily processed on RT-Thread.

### Sample Application Code

You may reach to [Here](http://117.143.63.254:9012/www/RT-AK/mnist_app.zip) for the sample code to run model reasoning mnist_app.c:

1. Download and decompress and place it under the `<BSP>/applications` path

![image-20210409192729527](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210409192730.png)



2. Select project engineering in `RT-Thread Studio` and right-click to refresh

![image-20210409193019579](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210524182808.png)



3. Compile and burn.

4. Enter the command: `mnsit_app`

![image-20210409192638719](https://gitee.com/lebhoryi/PicGoPictureBed/raw/master/img/20210524182937.png)

---

# 3. GitHub Address

https://github.com/EdgeAIWithRTT/Project3-Mnist_Cube_RTT/tree/master/Mnist_RTT