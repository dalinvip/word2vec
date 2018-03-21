# encoding=utf-8

import re
import csv
import urllib.request as urllib2
import urllib
from bs4 import BeautifulSoup
# solve encoding
from imp import reload
import sys
defaultencoding = 'utf-8'
if sys.getdefaultencoding() != defaultencoding:
    reload(sys)
    sys.setdefaultencoding(defaultencoding)


class Radical(object):
    dictionary_filepath = './Radical/xinhua.csv'
    # dictionary_filepath = './xinhua.csv'
    baiduhanyu_url = 'http://hanyu.baidu.com/zici/s?ptype=zici&wd=%s'

    def __init__(self):
        self.read_dictionary()
        self.origin_len = len(self.dictionary)

    def read_dictionary(self):
        self.dictionary = {}

        file = open(self.dictionary_filepath, encoding="UTF-8")
        reader = csv.reader(file)

        for line in reader:
            # print(line)
            self.dictionary[line[0]] = line[1]

        file.close()

    def write_dictionary(self):
        file_obj = open(self.dictionary_filepath, 'wb')

        writer = csv.writer(file_obj)
        for word in self.dictionary:
            writer.writerow([word, self.dictionary[word]])

        file_obj.close()

    def get_radical(self, word):
        if word in self.dictionary:
            return self.dictionary[word]
        else:
            # word = word.encode("utf-8")
            word = urllib.parse.quote(word)
            return self.get_radical_from_baiduhanyu(word)

    def post_baidu(self,url):
        try:
            timeout = 5
            request = urllib2.Request(url)
            request.add_header('User-agent', 'Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36')
            request.add_header('connection','keep-alive')
            request.add_header('referer', url)
            response = urllib2.urlopen(request, timeout=timeout)
            html = response.read()
            response.close()
            return html
        except Exception as e:
            print('URL Request Error:', e)
            return None

    def anlysis_radical_from_html(self, html_doc):
        soup = BeautifulSoup(html_doc, 'html.parser')
        li = soup.find(id="radical")
        radical = li.span.contents[0]

        return radical

    def add_in_dictionary(self,word,radical):
        # add in file
        file_object = open(self.dictionary_filepath,'a+')
        file_object.write(word+','+radical+'\r\n')
        file_object.close()

        # refresh dictionary
        self.read_in_dictionary()

    def get_radical_from_baiduhanyu(self,word):
        # url = self.baiduhanyu_url % word.decode("utf-8")
        url = self.baiduhanyu_url % word
        # url = self.baiduhanyu_url
        print(url, end=" ")
        html = self.post_baidu(url)
        # print(html)
        if html == None:
            return None
        radical = self.anlysis_radical_from_html(html)
        if radical != None:
            self.dictionary[word] = radical
        print(radical)
        return radical

    def save(self):
        if len(self.dictionary) > self.origin_len:
            self.write_dictionary()


if __name__ == '__main__':
    r = Radical()
    # print("word {}, radical {}".format("淥", r.get_radical("w")))
    print("word {}, radical {}".format("淥", r.get_radical("淥")))
    print("word {}, radical {}".format("中", r.get_radical("中")))
    print("word {}, radical {}".format("棶", r.get_radical("棶")))

    # r.save()



