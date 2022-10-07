## MicroPython 固件开发指南

如果手上没有官方支持固件的开发板，就需要自己来动手制作 MicroPython 固件了。由于 RT-Thread 官方提供了 [MicroPython 软件包](https://github.com/RT-Thread-packages/micropython)，并且 MicroPython 底层和硬件绑定时对接了 RT-Thread 驱动框架，因此可以很方便地在运行了 RT-Thread 的板卡上将 MicroPython 跑起来。

**注意**：RT-Thread MicroPython 需要运行在 **RT-Thread 3.0** 版本以上。

### 选择合适的 BSP 平台

RT-Thread MicroPython mini 版本占用资源最大不超过：

- ROM : 190KB
- RAM :  20KB

只要系统资源满足上述要求，常见的许多开发板都可以运行 MicroPython，例如 STM32 系列 BSP。

接下来我们以 `rt-thread\bsp\stm32\stm32f407-atk-explorer` 上的 MDK 工程为例，讲解如何在 BSP 的基础上制作 MicroPython 固件。

### 获取 MicroPython 软件包

先使用 `pkgs --upgrade` 命令更新软件包列表，然后通过 env 工具选中 MicroPython 软件包，最后使用 `pkgs --update` 命令将软件包拉取到本地。

![select_mpy_package](assets/select_mpy_package.png)

### 增大 main 线程栈

为了能后续在 main 线程中启动 MicroPython 运行时环境，需要增大 main 线程的栈大小，这里我们将栈大小增加到 8k。

![add_main_stack](assets/add_main_stack.png)

### 配置 MicroPython 运行环境堆大小

接下来根据板卡内存实际剩余情况来给 MicroPython 运行环境分配内存，这里填写的数值越大，就能运行更大代码量的 Python 程序。但是如果这里填写的数值超过了实际可分配内存，就可能会出现无法分配内存而报错。因此在配置此项目之前，需要对系统 RAM 资源的分配情况有一定了解。

#### 查看系统剩余内存

重新生成工程，编译下载后通过 `msh` 的 `free` 命令来查看内存使用情况。

![check_memory](assets/check_memory.png)

#### 配置系统

通过上一步查询的内存分配情况，对系统 RAM 资源有了一定的了解。在本次示例中，我们分配 20k 内存用于 MicroPython 运行时环境。后续如果想要运行更多 MicroPython 代码，可以将更多空余内存分配给 MicroPython 运行时环境，配置如下图所示：

![config_runtime](assets/config_runtime.png)

### 在系统根目录挂载文件系统

最后要确保系统中 `/` 目录挂载了文件系统。有了文件系统，后续才能使用 [**MicroPython 开发环境**](https://marketplace.visualstudio.com/items?itemName=RT-Thread.rt-thread-micropython) 将 Python 代码文件同步到板卡中来运行。

1. 打开 MicroPython 的文件同步功能选项

![open filesync option](assets/open_filesync_option.png)

2. 本次示例使用的开发板，文件系统存放在 SPI Flash 上，BSP 对该存储设备的支持已经做好了，在这里只需开启 elm-fat 文件系统即可，对系统进行如下配置：

![mount_fs](assets/mount_fs.png)

配置完成后，记得要使用 `scons --target=mkd5` 重新生成工程，使配置在工程中生效。

当你在自己的板卡上运行 MicroPython 时，你可以自由选择文件系统的存储介质，但是有一点很重要，文件系统要被挂载到根目录 / 上，这样才能保证在后续使用 MicroPython IDE 进行文件传输时不会出错。

### 在 main 线程中启动 MicroPython 

最后要在 main 线程中启动 MicroPython，代码修改如下所示：

```c
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <dfs_fs.h>
#include <rtdevice.h>

/* 文件系统所在分区名称，根据实际情况填写 */
#define FS_PARTITION_NAME     "W25Q128"

int main(void)
{
    /* 挂载 elm 文件系统到 / 目录，如果你所使用的开发板没有文件系统也可以跳过这一步 */
    if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("Filesystem initialized!");
    }
    else
    {
        /* 如果挂载失败，则尝试在文件系统分区重新创建文件系统 */
        dfs_mkfs("elm", FS_PARTITION_NAME);
        /* 尝试重新挂载文件系统 */
        if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) == 0)
        {
            /* 仍然挂载文件系统失败，请自行检查失败原因 */
            rt_kprintf("Failed to initialize filesystem!");
        }
    }

    rt_thread_mdelay(100);

    while(1)
    {
        /* 在这里让程序进入循环，通过这种方式实现 MicroPython 的软复位*/
        extern void mpy_main(const char *filename);
         /* 启动 MicroPython */
        mpy_main(NULL);
    }

    return RT_EOK;
}
```

重新编译工程并下载程序到板卡中，就会在 main 线程中自动启动 MicroPython，接下来就可以使用 [**RT-Thread MicroPython 开发环境**](https://marketplace.visualstudio.com/items?itemName=RT-Thread.rt-thread-micropython) 来进行应用开发了。 通过开发环境连接到开发板，即可看到 MicroPython 的交互环境 REPL，如下图所示：

![en_connect_board](assets/en_connect_board.gif)

