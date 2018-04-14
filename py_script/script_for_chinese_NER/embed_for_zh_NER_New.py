# @Author : bamtercelboo
# @Datetime : 2018/4/6 8:44
# @File : embed_for_zh_NER.py
# @Last Modify Time : 2018/4/6 8:44
# @Contact : bamtercelboo@{gmail.com, 163.com}

"""
    FILE :  embed_for_zh_NER.py
    FUNCTION : None
"""

import os
import sys
import numpy as np


class SourceFeat(object):
    def __init__(self, input_file, feat_file, source_embed_file, feat_embed_file, out_file):
        self.input_file = input_file
        self.feat_file = feat_file
        self.source_embed_file = source_embed_file
        self.feat_embed_file = feat_embed_file
        self.out_file = out_file

        self.corpus_dict = {}
        self.feat_dict = {}
        self.embed_source = {}
        self.embed_feat = {}
        self.avgembed_dict = {}

        self.read(infile=self.input_file)
        self.read_feat(infile=self.feat_file)
        self.read_embed(embed_dict=self.embed_source, embed_file=self.source_embed_file)
        self.read_embed(embed_dict=self.embed_feat, embed_file=self.feat_embed_file)
        self.avgembed_source_feat()
        self.write_dict(out_file=self.out_file, write_dict=self.avgembed_dict)

    def read(self, infile=None):
        print("read file from {}".format(infile))
        with open(infile, encoding="UTF-8") as f:
            for line in f:
                if line == "\n":
                    continue
                line = line.strip("\n").split(" ")
                self.corpus_dict[line[0]] = 1
        print("all {} chinese words".format(len(self.corpus_dict)))

    def read_feat(self, infile=None):
        print("read feature file from {}".format(infile))
        with open(infile, encoding="UTF-8") as f:
            for line in f:
                if line == "\n":
                    continue
                line = line.strip("\n")
                feat = str(line[2:])
                feat = feat.replace(" ", "")
                assert len(line) > 1
                self.feat_dict[line[0]] = feat
        print("all {} feature".format(len(self.feat_dict)))

    def read_embed(self, embed_dict, embed_file):
        print("read embedding file from {}".format(embed_file))
        with open(embed_file, encoding="UTF-8") as f:
            for line in f:
                line = line.split(" ")
                if "\n" in line:
                    line.remove("\n")
                embed_dict[line[0]] = line[1:]
        print("embed line {}".format(len(embed_dict)))

    def avgembed_source_feat(self):
        print("avg source and feature embedding")
        all_word = len(self.corpus_dict)
        now_line = 0
        for word in self.corpus_dict:
            now_line += 1
            sys.stdout.write("\rhandling with the {} line, all {} lines.".format(now_line, all_word))
            # if now_line == 100:
            #     break
            word_embed, feat_embed, num = 0, 0, 0
            if word in self.embed_source:
                num += 1
                word_embed = self.embed_source[word]
                # print(word_embed)
                word_embed = [float(i) for i in word_embed]
            if word in self.feat_dict:
                feat = self.feat_dict[word]
                sum_feat_embed, count = self.find_feat_embed(feat)
                if count != 0:
                    feat_embed = sum_feat_embed
                    num += count
            if word_embed is not 0 or feat_embed is not 0:
                if num is 0:
                    continue
                avg_embed = np.round(np.add(word_embed, feat_embed) / num, 6)
                avg_embed = [str(i) for i in avg_embed.tolist()]
                self.avgembed_dict[word] = avg_embed
        print("\navg embed finished, all {} words".format(len(self.avgembed_dict)))

    def find_feat_embed(self, feat):
        feat = "<" + feat + ">"
        feat_list = []
        for i in range(len(feat)):
            for j in range(3, 7):
                if (i + j) > len(feat):
                    continue
                sub_feat = feat[i:(i+j)]
                if sub_feat in self.embed_feat:
                    feat_list.append([float(i) for i in self.embed_feat[sub_feat]])
        count = len(feat_list)
        sum_feat = np.sum(feat_list, axis=0)
        return sum_feat, count

    def write_dict(self, out_file=None, write_dict=None):
        print("save to {}".format(out_file))
        if os.path.exists(out_file):
            os.remove(out_file)
        file = open(out_file, encoding="UTF-8", mode="w")
        for key, value in write_dict.items():
            # print("key {}, value {}".format(key, str(value)))
            v_str = self.dict_value2str(value)
            file.write(key + " " + v_str[1:] + "\n")
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
    print("handle source and feature embedding")
    input_file = "./Data/MSRA_All.txt"
    # feat_file = "./Data/merge_char_radical.txt"
    # feat_file = "./Data/merge_char_component.txt"
    feat_file = "./Data/merge_char_component_radical.txt"
    # feat_file = "./Data/char_component.txt"
    # feat_file = "./Data/char_component_radical.txt"
    out_file = "./Data/zhwiki_subcomponentf.100d.msra.target_feat_new.txt"
    source_embed = "./Data/zhwiki_subcomponentf.100d.target"
    feat_embed = "./Data/zhwiki_subcomponentf.100d.feature"

    SourceFeat(input_file, feat_file, source_embed, feat_embed, out_file)
    print("All handled.")
