# coding=utf-8
'''
@ Summary: Base parser about RT-AK tools
@ Update:

@ file:    base_parser.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2020/12/22 20:50

@ Update:  move utils to rt_ai_example

@ Update:  add `--pull_repo_only`
@ Date:    2021/08/02
'''
import argparse


def base_parser():
    parser = argparse.ArgumentParser()

    parser.add_argument("--log", type=str, default="",
                        help="Saved logging")
    parser.add_argument("--project", type=str, default="",
                        help="Project dir path")
    parser.add_argument("--model", type=str, default="./Models/mnist.tflite",
                        help="indicates the original model file paths")
    parser.add_argument("--rt_ai_lib", type=str, default="../rt_ai_lib",
                        help="RT-Thread develop libiraies")
    parser.add_argument("--model_name", type=str, default="network",
                        help="The convert model name.")
    parser.add_argument("--pull_repo_only", type=bool, default=False,
                        help="If true, only git pull plugin platform repository.")
    parser.add_argument("--support_platforms", type=str, default="./platforms/support_platforms.json",
                        help="Support platforms repositorys")
    parser.add_argument("--platform", type=str, default="example",
                        help="Platform used, eg:stm32, k210, example")
    return parser
