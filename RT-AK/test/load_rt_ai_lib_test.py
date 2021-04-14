# coding=utf-8
'''
@ Summary: load rt_ai_lib test to project
@ Update:  

@ file:    load_rt_ai_lib_test.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/3/20 17:16
'''
from pathlib import Path

def add_rt_ai_libs_test(project, rt_ai_libs_test):
    sconstruct = Path(project) / "SConstruct"
    with sconstruct.open() as fr:
        lines = fr.readlines()

    if rt_ai_libs_test[0] in lines:
        print("Don't need to add rt_ai_libs test case to project again!!!")
        return

    result = list()  # add rt_ai_libs test case to sconstruct
    for _, line in enumerate(lines):
        if "sys.path" in line:
            result.append(rt_ai_libs_test[0])
            result.append(rt_ai_libs_test[1])
        elif "DoBuilding" in line:
            result.append(rt_ai_libs_test[2])
        result.append(line)

    with sconstruct.open("w+") as fw:
        fw.write("".join(result))
    print("Adding rt_ai_libs test case to project successfully...")