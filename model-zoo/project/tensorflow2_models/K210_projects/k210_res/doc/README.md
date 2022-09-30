# Draco开发板MicroPython扩展

Draco开发板MicroPython设备控制相关模块说明: [MicroPython.md](./MicroPython.md)

RT-AK-MicroPython模块使用说明: [RT-AK-MicroPython.md](./RT-AK-MicroPython.md)

RT-AK-MicroPython模块使用和设计说明: [../mpy_rt_ai/READMEmd](../mpy_rt_ai/README.md)

MicroPython运行时需要从系统分配一块内存用于MicroPython自动管理。内存大小可在`RT-Thread Settings->软件包->语言包->MicroPython->Heap size for python run environment` 选项中配置
- 仅使用OpenMV时可分配较大内存，如2000000（Bytes）。
- 若运行AI时，则建议减小MicroPython的运行内存，以保证AI运行时有足够堆内存可分配。
