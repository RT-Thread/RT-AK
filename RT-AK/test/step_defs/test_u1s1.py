# coding=utf-8
'''
@ Summary: 
@ Update:  

@ file:    test_u1s1.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/3/19 21:51
'''
import os
import time
import pytest
import shutil
from pathlib import Path
from functools import partial
from multiprocessing import cpu_count
from pytest_bdd import scenario, given, when, then, parser

from download_file import pre_bsp_gnu_exttools
from load_rt_ai_lib_test import add_rt_ai_libs_test


def readonly_handler(func, path, execinfo):
    # Change the mode of file, to make it could be used of shutil.rmtree
    os.chmod(path, 128)
    func(path)


@pytest.fixture(scope="module", autouse=True)
def setup_teardown():
    """ setup & teardown job
    setup:
        1. download files, include: bsp/ext_tools/windows
        2. unzip files
        3. set env
    tesardown:
        remove tmp_work_dir
    """
    tmp_work_dir = Path(os.getenv("TMP_DIR")).resolve()
    _ = pre_bsp_gnu_exttools(str(tmp_work_dir))
    print(f"tmp_work_dir: {tmp_work_dir}")
    yield
    os.chdir(tmp_work_dir.parent)
    shutil.rmtree(tmp_work_dir, onerror=readonly_handler)


scenario_user = partial(scenario, "../features/U1S1.feature")
given_user = partial(given, target_fixture="pre_data")

@scenario_user("run rt_ai_tools")
def test_1():
    pass

@scenario_user("load rt_ai_lib test to project")
def test_2():
    pass


# given

@given_user("there is a <model> base on <platform>")
def pre_data_ai_tools(model, platform):
    model = Path(model).resolve()
    assert model.exists(), FileNotFoundError(f"{model.name} not found!!!")
    assert platform == "stm32", (f"wrong {platform}!!!")

    params = dict(
        model = str(model),
        platform = platform,

        # init
        rt_ai_tools = "../rt_ai_tools",
        aitools_py = "aitools.py",
        model_name = "mnist",
        rt_ai_tools_abspath = Path("../rt_ai_tools").parent.cwd(),
        project = os.getenv("BSP"),
        ext_tools = os.getenv("EXT_TOOLS"),
    )
    return params


@given_user("there is a project")
def pre_data_lib_test():
    test_path = Path(__file__).parent.parent.resolve()
    rt_ai_libs_test = [
        f'if not os.getenv("RTAK_TEST"):\n',
        f'    RTAK_TEST=r"{test_path}"\n\n',
        "objs.extend(SConscript(os.path.join(RTAK_TEST, 'Sconscript')))\n"
    ]
    sconstruct = Path(os.getenv("BSP")) / "SConstruct"
    os.environ["RTT_EXEC_PATH"] = os.getenv("GNU")
    return dict(project=os.getenv("BSP"), rt_ai_libs_test=rt_ai_libs_test,
                sconstruct=sconstruct)


# when 

@when("I run aitools")
def run_ai_tools(pre_data):
    # switch path to "RTAK/rt_ai_root"
    os.chdir(pre_data["rt_ai_tools"])
    tools_cmd = f'python {pre_data["aitools_py"]} --project={pre_data["project"]} ' \
                f'--model={pre_data["model"]} --model_name={pre_data["model_name"]} ' \
                f'--platform={pre_data["platform"]} --ext_tools={pre_data["ext_tools"]} --clear'
    output = os.popen(tools_cmd)
    _ = output.read()
    output.close()


@when("add rt_ai_lib to project")
def run_load_lib_test(pre_data):
    _ = add_rt_ai_libs_test(pre_data['project'], pre_data['rt_ai_libs_test'])


@when("I complie the project")
def run_compile(pre_data):
    os.chdir(pre_data['project'])

    # 如果之前有编译过，先 scons -c
    if "cconfig.h" in os.listdir():
        os.remove("rtthread.bin")
        with os.popen("scons -c") as f:
            f.read()

    cpus = cpu_count()
    compile_cmd = f"scons -j {cpus}"
    print(compile_cmd)
    with os.popen(compile_cmd) as f:
        print(f.read())


# then 

@then("I would get the <dir> in project")
def val_ai_tools(pre_data, dir):
    rt_ai_lib_path = Path(pre_data["project"]) / dir
    assert rt_ai_lib_path.exists()


@then('I would get a SConstruct file include "RTAK_TEST"')
def val_ai_lib_test(pre_data,):
    with pre_data['sconstruct'].open() as f:
        lines = f.readlines()

    line = [i for i in lines if "RTAK_TEST" in i]
    assert line


@then("I would get a <firmware>")
def val_compile(firmware):
    assert firmware in os.listdir(), FileNotFoundError(f"{firmware} not found!!")
