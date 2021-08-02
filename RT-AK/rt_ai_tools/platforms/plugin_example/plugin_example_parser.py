# coding=utf-8
'''
@ Summary: 
@ Update:  

@ file:    plugin_example_parser.py
@ version: 1.0.0

@ Author:  Lebhoryi@gmail.com
@ Date:    2021/8/2 18:33
'''
def platform_parameters(parser):
    """exmaple platform"""
    parser.add_argument("--example", type=str, default="example",
                        help="The exmaple parser of sub plugin parsers.")
    parser.add_argument("--enable_rt_lib", type=str, default="RT_AI_USE_Example",
                        help="Enabel RT-AK Lib using sub plugin.")
    return parser
