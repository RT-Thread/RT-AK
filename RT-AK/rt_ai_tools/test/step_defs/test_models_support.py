# coding=utf-8
'''
@ Summary: 测试文档中是否说明 RT-AK 中支持模型的格式
@ Update:  

@ file:    test_is_model_support_in_file.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/3/8 17:05
'''
import os
import pytest
from pytest_bdd import scenarios, given, when, then, parser


scenarios("../features/u1s1_3.feature", )
# def test_is_model_support():
#     pass


# Given Steps
@given("I got one file <file>", target_fixture="get_data")
def get_data(file):
    return dict(file=file)


# When Steps
@when("I read the file")
def read_file(get_data):
    count_sup_plats = 0  # 支持的平台数量
    with open(get_data["file"], "r+", encoding="utf-8") as f:
        lines = f.readlines()

    for line in lines:
        if "模型支持" in line:
            count_sup_plats += 1
    get_data["count_sup_plats"] = count_sup_plats


# Then Steps
@then("the results should have some platform support")
def see_result(get_data):
    assert get_data["count_sup_plats"]


@then("the file could not be read")
def wrong_file(get_data):
    try:
        f = open(get_data['file'])
    except FileNotFoundError as fe:
        assert "No such file or directory" in str(fe)
    else:
        f.close()


@then("There are too many <platforms>")
def wrong_platforms(get_data, platforms):
    platform_nums = len(platforms.split())
    assert get_data["count_sup_plats"] != platform_nums, "目标平台模型支持不完全2，请及时更新！！！"
