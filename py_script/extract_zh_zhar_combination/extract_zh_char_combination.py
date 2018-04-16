# @Author : bamtercelboo
# @Datetime : 2018/4/14 19:40
# @File : extract_zh_char_combination.py
# @Last Modify Time : 2018/4/14 19:40
# @Contact : bamtercelboo@{gmail.com, 163.com}

"""
    FILE :  extract_zh_char_combination.py
    FUNCTION : None
"""

import os
import sys


class Combination(object):
    def __init__(self, in_file, out_file):
        print("extract chinese character combination")
        self.in_file = in_file
        self.out_file = out_file
        self.corpus_dict = {}
        self.char_combination_dict = {}
        self.read_file(in_file)
        self.write_dict()

    def read_file(self, infile):
        with open(infile, encoding="Utf-8") as f:
            for line in f:
                line = line.strip("\n")
                word, feat = "", ""
                word, _, feat = line.partition("|")
                feat = feat.replace(" ", "")
                self.char_combination_dict[word] = feat

    def write_dict(self):
        if os.path.exists(self.out_file):
            os.remove(self.out_file)
        file = open(self.out_file, encoding="UTF-8", mode="w")
        for word, feat in self.char_combination_dict.items():
            feat = self.add_space(feat)
            file.writelines(word + " " + feat + "\n")


    def add_space(self, str):
        space_str = ""
        for char in str:
            space_str += (char + " ")
        return space_str[:-1]



if __name__ == "__main__":

    input_file = "./Data/char2elements_map.txt"
    output_file = "./Data/char_combination.txt"
    Combination(in_file=input_file, out_file=output_file)

