# RT-AK Contribution Guide

Your effort to improve `RT-AK` is highly appriciated!

## 1. RT-AK main repo

Please follow google python coding rules and ensure your code pass the CI.

## 2. Platform Plugin

Please create a stand-alone repo for the plugin, then add url into  `platforms/support_platforms.json`

The repo of plugin usually contains:

1. `RT-AK Lib`

   Adapt `RT-AK`  with target platform, like inference engine and hardware.

    * Naming: `backend_xxx`
    * Position at development: root of the plugin repo
    * Position at working: rt_ai_tools/platforms/<platform>`
    * Example: `rt_ai_tools/platforms/example/backend_example`

2. `RT-AK Tools`

  So far  `python` scripts to convert model to proper format, load  `RT-AK Lib` and glue code into BSP project.

    - Naming: `<platform>`
    - Position at development: root of the plugin repo
    - Postion at working: `rt_ai_tools/platforms`
    - Example: `rt_ai_tools/platforms/example`

3. Test cases

4. Document for users and hackers

For more details of these two parts, please refer readme in corresponding dir.

## 3. Setup

* clone latest revision of this `RT-AK` repo

## 4. Development

1. Please refer the example, implment all the interfaces, please use google python coding rules and RT-Thread C coding rules.
2. Please keep git commit message meaningful.
3. Kepp the code simple, easy to read. Only enable neccessary items in `menuconfig` if invovled.
4. Write a detailed `README`
   - For every routine function
     - explain its purpose and usage, better with example
     - if there is anything need special care, write down clearly
   - Sign your name and contacts.

## 5. PR

- Once you have done the development, you may submit it, coordinators will review it then accept or request for changes.
- Please push your code to your clone of RT-AK repo and plugin repo if involved, wait CI passed, create a pull request and assign to the coordinator.
- Take care  review notification until all codes get merged.

