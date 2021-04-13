# coding=utf-8
"""
@ Summary: ai platform
@ Update:  

@ file:    utils.py
@ version: v1.0.2

@ Author:  Lebhoryi@gmail.com
@ Date:    2020/12/23 16:36

@ Update:  remove update_info.load_rt_ai_example
@ Date:    20210223

@ Update:  remove sample_main function
@ Date:    2021/03/12

@ Update:  add merge submodules
@ Date:    2021/04/02
"""
import shutil
import logging
from pathlib import Path
from importlib import import_module

from utils import base_parser
from utils import install_plugin



def load_sub_parsers(parser, platform:str):
    """ parsing sub(plugin) argparsers
    Args:
        parser: base parsers, Namespace
        platform: which the platform you choosed, str

    Returns:
        opt: all parsers, Namespace

    """
    sub_module = f"platforms.{platform}.{platform}_parser"
    sub_parser = import_module(sub_module)
    parser = sub_parser.platform_parameters(parser)
    # parsing all parameters
    opt = parser.parse_args()
    return opt


def enable_platform(rtconfig, enable_lib):
    """ add '#define enbale_lib' to rtconfig.h"""
    rtconfig_path = Path(rtconfig) / "rtconfig.h"
    assert rtconfig_path.exists(), "No rtconfig.h found!"
    assert enable_lib, "No platform define found!"

    with rtconfig_path.open("r+") as fr:
        lines = fr.readlines()

    new_line = f"#define {enable_lib}\n"

    if new_line in lines:
        logging.info("No need to enble RT-AK Lib again...")
        return

    for index, line in enumerate(lines):
        if "RT_AI" in line:
            lines.remove(line)
            break

    lines.insert(-2, new_line)
    lines = "".join(lines)

    with rtconfig_path.open("w+") as fw:
        fw.write(lines)

    logging.info(f"Enable RT-AK Lib {enable_lib} successfully...")


def load_rt_ai_lib(project, rt_ai_lib, platform):
    """ Copy RT-AK Lib to project
        1. copy export plugin lib to RT-AK Lib
        2. copy RT-AK Lib to project

    Args:
        project: BSP, str
        rt_ai_lib: RT-AK Lib path
    """
    rt_ai_lib = Path(rt_ai_lib)
    # RT-AK Lib dir name
    lib_name = rt_ai_lib.name
    plugin_lib_name = f"backend_{platform}"
    # plugin lib dir; c codes
    plugin_lib = Path()/f"platforms/{platform}/{plugin_lib_name}"
    # RT-AK Lib in project
    lib_path = Path(project) / lib_name

    # copy rt_ai_lib to project
    if lib_path.exists():
        shutil.rmtree(lib_path)
    shutil.copytree(rt_ai_lib, lib_path)

    # copy plugin lib to RT-AK Lib
    for path in lib_path.iterdir():
        if path.is_dir() and "backend" in path.name:
            shutil.rmtree(path)
    shutil.copytree(plugin_lib, lib_path/plugin_lib_name)

    logging.info("RT-AK Libs loading successfully...")


def main():
    ####################################################################
    #
    # 1. loadding base parsers
    #
    ####################################################################
    parser = base_parser.base_parser()

    # parsing base parameters
    opt, unknown = parser.parse_known_args()

    # alis platform name to 'plugin_<platform>'
    platform_using = f"plugin_{opt.platform}"


    ####################################################################
    #
    # 2. setting logs
    #
    ####################################################################
    logging.basicConfig(filename=opt.log, filemode="w",
        format="%(asctime)s %(funcName)s [%(levelname)s]: %(message)s",
        datefmt="%Y-%m-%d %H:%M:%S",
        level=logging.INFO)


    ####################################################################
    #
    # 3. git clone/pull plugin repo
    #
    ####################################################################
    install_plugin.git_plugin(opt.support_platforms, platform_using)


    ####################################################################
    #
    # 4. loadding plugin parsers
    #
    ####################################################################
    # get all dirs in rt_ai_tools/platforms
    platforms = [platform.name for platform in Path("platforms").iterdir() if platform.is_dir()]
    assert platform_using in platforms, \
        f"{opt.platform} not found in './platforms'"

    opt = load_sub_parsers(parser, platform_using)


    ####################################################################
    #
    # 5. run plugin base on platform
    #
    ####################################################################
    plugin_path = f"platforms.{platform_using}.{platform_using}"
    plugin_module = import_module(plugin_path)
    Plugin = plugin_module.Plugin(opt)  # import class
    _ = Plugin.run_plugin()


    ####################################################################
    #
    # 6. enable RT-AK Lib using the third platform
    #
    ####################################################################
    _ = enable_platform(opt.project, opt.enable_rt_lib)


    ####################################################################
    #
    # 7. load RT-AK Lib to project
    #
    ####################################################################
    _ = load_rt_ai_lib(opt.project, opt.rt_ai_lib, platform_using)


if __name__ == "__main__":
    main()