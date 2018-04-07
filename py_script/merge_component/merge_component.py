# @Author : bamtercelboo
# @Datetime : 2018/4/7 14:46
# @File : merge_component.py
# @Last Modify Time : 2018/4/7 14:46
# @Contact : bamtercelboo@{gmail.com, 163.com}

"""
    FILE :  merge_component.py
    FUNCTION : None
"""
import os
import sys

class MergeComponent(object):
    def __init__(self, file1, file2, out_file):
        print("merge component")
        self.file1 = file1
        self.file2 = file2
        self.out_file = out_file
        self.file1_dict = {}
        self.file2_dict = {}
        self.out_dict = {}
        self.read(self.file1, self.file1_dict)
        self.read(self.file2, self.file2_dict)
        self.merge()
        self.write_dict(self.out_file, self.out_dict)

    def read(self, infile, dict):
        with open(infile, encoding="UTF-8") as f:
            for line in f:
                if line == "\n":
                    continue
                line = line.strip("\n")
                # print(line)
                word = line[0]
                feat = line[1:].replace(" ", "")
                dict[word] = feat
        print("all {} words".format(len(dict)))

    def merge(self):
        for word in self.file1_dict:
            feat = self.file1_dict[word]
            if word in self.file2_dict:
                feat += self.file2_dict[word]
            self.out_dict[word] = feat
        print("all {} words".format(len(self.out_dict)))

    def write_dict(self, out_file=None, write_dict=None):
        print("save to {}".format(out_file))
        if os.path.exists(out_file):
            os.remove(out_file)
        file = open(out_file, encoding="UTF-8", mode="w")
        for key, value in write_dict.items():
            print("key {}, value {}".format(key, str(value)))
            # v_str = self.dict_value2str(value)
            file.write(key + " " + value + "\n")
        file.close()
        print("Save Finished.")

    def dict_value2str(self, v_list=None):
        if v_list is None:
            return ""
        if isinstance(v_list, list) is False:
            return ""
        v_str = ""
        for v in v_list:
            v_str += (" " + v)
        return v_str


if __name__ == "__main__":
    file1 = "./Data/char_radical.txt"
    file2 = "./Data/char_component.txt"
    out_file = "./Data/char_component_radical.txt"
    MergeComponent(file1, file2, out_file)





