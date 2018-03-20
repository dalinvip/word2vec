#encoding:utf-8
# @Author : bamtercelboo
# @Datetime : 2018/3/20 10:37
# @File : extract_zh_char_radical.py
# @Last Modify Time : 2018/3/20 10:37
# @Contact : bamtercelboo@{gmail.com, 163.com}

"""
    FILE :  extract_zh_char_radical.py
    FUNCTION : None
    EXAMPLE:
        Source:
        中国国庆假期香江将涌入人潮
        TO:
        中_丨 国_囗 国_囗 庆_广 假_亻 期_月 香_香 江_氵 将_寸 涌_氵 入_入 人_人 潮_氵
"""

import os
import sys


def read(input_file=None):
    print("read file from {}".format(input_file))
    line_list = []
    count_line = 0
    with open(input_file, encoding="UTF-8") as f:
        for line in f:
            if line == "\n":
                continue
            count_line += 1
            line_list.append(line.strip("\n"))
    print("Read Finished, all {} lines.".format(count_line))
    return line_list, count_line


def extract_char(line_list=None):
    print("extract char to dict......")
    char_dict = {}
    all_line = len(line_list)
    now_line = 0
    for line in line_list:
        now_line += 1
        sys.stdout.write("\rhandling with the {} line, all {} lines.".format(now_line, all_line))
        for char in line:
            if char in dict:
                continue
            char_dict[char] = 1
    print("\nextract char to dict finished.")
    return char_dict, len(char_dict)


if __name__ == "__main__":
    print("extract chinese char radical")
    input_file = "./Data/giga_small.txt"
    line_list, count_line = read(input_file=input_file)
    dict, char_count = extract_char(line_list=line_list)



