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
from optparse import OptionParser
sys.path.append(".")
from Radical.radical import Radical

NO_RADICAL_FLAG = "NRA"
SPLIT_RADICAL = "_"

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
            if char in char_dict:
                continue
            char_dict[char] = 1
    print("\nextract char to dict finished.")
    return char_dict, len(char_dict)


def is_chinese(uchar):
    """判断一个unicode是否是汉字"""
    if (uchar >= u'\u4e00') and (uchar <= u'\u9fa5'):
        return True
    else:
        return False


def get_char_radical(char_dict=None, char_count=None):
    print("get char radical......")
    r = Radical()
    radical_dict = {}
    radical_nocover = {}
    now_line = 0
    for char in char_dict:
        now_line += 1
        sys.stdout.write("\rhandling with the {} count, all {} counts.".format(now_line, char_count))
        if is_chinese(char) is False:
            radical_nocover[char] = NO_RADICAL_FLAG
            continue
        ra = r.get_radical(char)
        # print(ra)
        if ra is None:
            radical_nocover[char] = NO_RADICAL_FLAG
            continue
        radical_dict[char] = ra

    print("\nHandle Finished.")
    print("Find radical {} numbers, Not find {} numbers, all {} numbers".format(len(radical_dict),
                                                                                len(radical_nocover),
                                                                                len(radical_dict) + len(radical_nocover)))
    return radical_dict, radical_nocover


def save_radical(radical_file=None, ra_dict=None):
    print("save radical to {}".format(radical_file))
    if os.path.exists(radical_file):
        os.remove(radical_file)
    file = open(radical_file, encoding="UTF-8", mode="w")
    for key, value in ra_dict.items():
        # print("key {}, value {}".format(key, value))
        if is_chinese(key) is False:
            continue
        file.write(key + " " + value + "\n")
    file.close()
    print("Save Finished.")


def extract_corpus_char_radical(line_list=None, radical_dict=None):
    print("extract corpus char radical......")
    corpus_line = []
    now_line = 0
    for line in line_list:
        now_line += 1
        sys.stdout.write("\rhandling with the {} line, all {} lines.".format(now_line, count_line))
        new_line = ""
        # print(line)
        for char in line:
            if char in radical_dict:
                char += (SPLIT_RADICAL + radical_dict[char] + " ")
                new_line += char
            else:
                char += (SPLIT_RADICAL + NO_RADICAL_FLAG + " ")
                new_line += char
        corpus_line.append(new_line[:-1])
    print("\nextract corpus char radical finished.")
    return corpus_line


def save_corpus_char_radical(corpus_line=None, out_file=None):
    print("Saving corpus char radical.....")
    if os.path.exists(out_file):
        os.remove(out_file)
    file = open(out_file, encoding="UTF-8", mode="w")
    now_line = 0
    count_line = len(corpus_line)
    for line in corpus_line:
        now_line += 1
        sys.stdout.write("\rhandling with the {} line, all {} lines.".format(now_line, count_line))
        file.writelines(line + "\n")
    print("\nHandle Finished.")


if __name__ == "__main__":
    print("extract chinese char radical")
    input_file = "./Data/giga_small.txt"
    output_file = "./Data/giga_small_out.txt"
    save_radical_file = "./Data/radical"

    # parser = OptionParser()
    # parser.add_option("--input", dest="input", help="input file")
    # parser.add_option("--output", dest="output", help="output file")
    # parser.add_option("--save_radical", dest="save_radical", help="save radical file")
    # (options, args) = parser.parse_args()

    # input_file = options.input
    # output_file = options.output
    # save_radical_file = options.save_radical

    try:
        line_list, count_line = read(input_file=input_file)
        char_dict, char_count = extract_char(line_list=line_list)
        radical_dict, radical_nocover = get_char_radical(char_dict=char_dict, char_count=char_count)
        save_radical(radical_file=save_radical_file + ".Found", ra_dict=radical_dict)
        save_radical(radical_file=save_radical_file + ".NoFound", ra_dict=radical_nocover)
        corpus_line = extract_corpus_char_radical(line_list=line_list, radical_dict=radical_dict)
        save_corpus_char_radical(corpus_line=corpus_line, out_file=output_file)
    except Exception as err:
        print(err)
    print("All Finished.")












