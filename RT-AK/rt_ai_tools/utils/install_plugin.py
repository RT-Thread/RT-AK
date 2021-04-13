# coding=utf-8
'''
@ Summary: install plugin with git
@ Update:  

@ file:    install_plugin.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/4/8 13:53

@ Update:  move 'git pull --rebase' to 'remove dir'
@ Date:    2021/04/12
'''
import os
import json
import logging
import shutil
from pathlib import Path


def readonly_handler(func, path):
    # Change the mode of file, to make it could be used of shutil.rmtree
    os.chmod(path, 128)
    func(path)


def set_git_env(env_path):
    os.environ["PATH"] = env_path


def excute_cmd(cmd):
    p = os.popen(cmd)
    content = p.read()
    p.close()
    return content


def git_plugin(sup_file, platform):
    if platform == "plugin_example":  return

    raw_path = os.getcwd()
    abs_dir = os.path.join("platforms", platform)
    with open(sup_file, "r+") as f:
        sup_platforms_dict = json.load(f)

    # plugin url
    platform_url = sup_platforms_dict[platform]

    try:
        with os.popen("git --version") as p:
            logging.info(p.read().strip())
    except:
        raise Exception("Not git found!")

    if Path(abs_dir).exists() and list(Path(abs_dir).iterdir()):
        # swich to rt_ai_tools/platforms/<platform>
        os.chdir(abs_dir)

        cmds = ["git stash", "git pull --rebase", "git stash pop"]
        # logging.info(f"Starting 'git pull' from {platform_url}")
        for i, cmd in enumerate(cmds):
            content = excute_cmd(cmd)
            # if i == 1:
            #     logging.info(content)
        os.chdir(raw_path)
    else:
        # clone_cmd = f"git clone --depth=1 {platform_url} {abs_dir}"
        clone_cmd = f"git clone {platform_url} {abs_dir}"
        with os.popen(clone_cmd) as p:
            context = p.read()

    logging.info(f"Install {platform} successfully...")


if __name__ == "__main__":
    logging.getLogger().setLevel("INFO")
    file = "./platforms/support_platforms.json"
    platform = "plugin_stm32"
    tmp = "tmp"
    os.chdir("..")
    git_plugin(file, platform)