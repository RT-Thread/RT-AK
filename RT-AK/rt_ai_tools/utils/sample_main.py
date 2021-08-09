# coding=utf-8
'''
@ Summary: modify main.c
@ Update:

@ file:    sample_main.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2020/12/8 19:35

@ Update:  add sample_list[1:] into main()
@ Date:    2020/12/10
'''
import logging
from pathlib import Path

# which would be wrote to main.c
sample_list = ["#include <ai_app_template.h>\n\n",
               "\n    /* Example Here */\n\n",
               "    sample_network_init();\n",
               "    sample_network_process();\n",
               "    sample_deal();\n\n"]


def example_main(project, sample_list=sample_list):
    """ add example to bsp/main.c"""
    file_path = Path(project) / "applications/main.c"
    assert file_path.exists(), "No main.c!!!"

    with file_path.open() as fr:
        lines = fr.readlines()

    # "#include <ai_app_template.h>\n"
    if sample_list[0][:-1] in lines:
        logging.info("Don't need to modify main.c!!!")
        return file_path

    # save lines index about "define" and "main"
    index = list()
    for i, line in enumerate(lines):
        if "define" in line:
            index.append(i)
        if "main" in line:
            index.append(i+2)

    lines = \
        lines[:index[0]] + [sample_list[0]] + \
        lines[index[0]:index[1]] + sample_list[1:] + lines[index[1]:]

    with file_path.open("w") as fw:
        fw.write("".join(lines))

    logging.info("Now you hava an example main.c")

    return file_path


if __name__ == "__main__":
    import shutil

    logging.getLogger().setLevel(logging.INFO)

    project_path = r"D:\RT-ThreadStudio\workspace\art_pi\applications"
    main_file = Path(project_path) / 'main.c'

    tmp_project = Path("../tmp_cwd")
    app_path = tmp_project / "applications"

    if not app_path.exists():
        app_path.mkdir(parents=True)

    shutil.copy(main_file, app_path)

    example_main(tmp_project, sample_list)
    # print(sample_list[0][:-2])
