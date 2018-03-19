/**
* @Author: bamtercelboo
* @Date: 2018/03/15
* @File: dictionary.h
* @Contact: bamtercelboo@{gmail.com, 163.com}
* @Function: None
*/

#pragma once

#include "args.h"
#include "real.h"
#include "alphabet.h"

#include <random>
#include <memory>
#include <assert.h>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <cmath>


struct entry {
	std::string word;
	int64_t count;
	std::vector<int32_t> subwords;
};

class Dictionary {
  protected:
	static const int32_t MAX_VOCAB_SIZE = 100000000;
	static const int32_t MAX_LINE_SIZE = 1024;

	int32_t findWord(const std::string&) const;
	void addWord(const std::string&);
	int32_t findTarget(const std::string&) const;
	void addTarget(const std::string&, int64_t);
	int32_t findFeature(const std::string&) const;
	void addFeature(const std::string&, int64_t);

	void initFeature();
	void initTargets();
	void initNgrams();

	void reset(std::istream&) const;

	std::shared_ptr<Args> args_;
	alphabet words_;
	std::vector<entry> wordprops_;
	alphabet features_;
	alphabet targets_;
	std::vector<real> pdiscard_;
	int64_t ntokens_;

public:
	static const std::string EOS;
	static const std::string BOW;
	static const std::string EOW;

	explicit Dictionary(std::shared_ptr<Args>);

	int32_t nwords() const;
	int32_t ntargets() const;
	int32_t nfeatures() const;
	int64_t ntokens() const;
	int32_t getWordId(const std::string&) const;
	int32_t getTargetId(const std::string&) const;
	int32_t getFeatureId(const std::string&) const;
	std::string getWord(int32_t) const;
	std::string getTarget(int32_t) const;
	std::string getFeature(int32_t) const;

	std::vector<int64_t> getCounts() const;
	void computeSubwords(const std::string&, std::vector<std::string>&) const;
	void computeSubwords(const std::string&, std::vector<int32_t>&) const;

	void initTableDiscard();
	bool discard(int32_t, real) const;

	bool readWord(std::istream&, std::string&) const;
	void readFromFile(std::istream&);
	//int32_t getLine(std::istream&, std::vector<int32_t>&, std::minstd_rand&) const;
	int32_t getLine(std::istream&, std::vector<std::vector<int32_t> >&, std::vector<std::vector<int32_t> >&, std::vector<int32_t>&, std::minstd_rand&) const;
};

const std::string Dictionary::EOS = "</s>";
const std::string Dictionary::BOW = "<";
const std::string Dictionary::EOW = ">";

/**
* @Function: initial Dictionary class argument.
*/
Dictionary::Dictionary(std::shared_ptr<Args> args) : args_(args) {
	words_.setCapacity(MAX_VOCAB_SIZE - 1);
	features_.setCapacity(MAX_VOCAB_SIZE - 1);
	targets_.setCapacity(MAX_VOCAB_SIZE - 1);
}

/**
* @Function: find word Id.
*/
int32_t Dictionary::findWord(const std::string& w) const {
	return words_.from_string(w);
}

/**
* @Function: find word Id.
*/
int32_t Dictionary::getWordId(const std::string& w) const {
	return findWord(w);
}

/**
* @Function: find word from Id.
*/
std::string Dictionary::getWord(int32_t id) const {
	assert(id >= 0);
	assert(id <= words_.m_size);
	return words_.from_id(id);
}

/**
* @Function: add word to alphabet.
*/
void Dictionary::addWord(const std::string& w) {
	words_.add_string(w);
}

/**
* @Function: word count in alphabet.
*/
int32_t Dictionary::nwords() const {
	return words_.m_size;
}

/**
* @Function: find target Id.
*/
int32_t Dictionary::findTarget(const std::string& w) const {
	return targets_.from_string(w);
}

/**
* @Function: find target Id.
*/
int32_t Dictionary::getTargetId(const std::string& w) const {
	return findTarget(w);
}

/**
* @Function: find target from Id.
*/
std::string Dictionary::getTarget(int32_t id) const {
	assert(id >= 0);
	assert(id < targets_.m_size);
	return targets_.from_id(id);
}

/**
* @Function: add target to alphabet.
*/
void Dictionary::addTarget(const std::string& w, int64_t freq) {
	targets_.add_string(w);
}

/**
* @Function: target count in alphabet.
*/
int32_t Dictionary::ntargets() const {
	return targets_.m_size;
}

/**
* @Function: find feature Id.
*/
int32_t Dictionary::findFeature(const std::string& w) const {
	return features_.from_string(w);
}

/**
* @Function: find feature Id.
*/
int32_t Dictionary::getFeatureId(const std::string& w) const {
	return findFeature(w);
}

/**
* @Function: find feature from Id.
*/
std::string Dictionary::getFeature(int32_t id) const {
	assert(id >= 0);
	assert(id < features_.m_size);
	return features_.from_id(id);
}

/**
* @Function: add feature to alphabet.
*/
void Dictionary::addFeature(const std::string& w, int64_t freq) {
	features_.add_string(w);
}

/**
* @Function: feature count in alphabet.
*/
int32_t Dictionary::nfeatures() const {
	return features_.m_size;
}

/**
* @Function: target initial.
*/
void Dictionary::initTargets() {
	//same as source
	for (size_t i = 0; i < words_.m_size; i++) {
		addTarget(words_.from_id(i), words_.m_id_to_freq[i]);
	}
}

/**
* @Function: feature initial.
*/
void Dictionary::initFeature() {
	if (args_->model == model_name::skipgram)
		return;
	for (size_t i = 0; i < words_.m_size; i++) {
		std::string word = BOW + words_.from_id(i) + EOW;
		if (word != EOS) {
			vector<string> ngrams;
			computeSubwords(word, ngrams);
			for (size_t j = 0; j < ngrams.size(); j++) {
				addFeature(ngrams[j], words_.m_id_to_freq[i]);
			}
		}
	}
}

void Dictionary::computeSubwords(const std::string& word, std::vector<std::string>& substrings) const {
	for (size_t i = 0; i < word.size(); i++) {
		std::string ngram;
		if ((word[i] & 0xC0) == 0x80) continue;
		for (size_t j = i, n = 1; j < word.size() && n <= args_->maxn; n++) {
			ngram.push_back(word[j++]);
			while (j < word.size() && (word[j] & 0xC0) == 0x80) {
				ngram.push_back(word[j++]);
			}
			if (n >= args_->minn && !(n == 1 && (i == 0 || j == word.size()))) {
				substrings.push_back(ngram);
			}
		}
	}
}


void Dictionary::computeSubwords(const std::string& word, std::vector<int32_t>& ngrams) const {
	for (size_t i = 0; i < word.size(); i++) {
		std::string ngram;
		if ((word[i] & 0xC0) == 0x80) continue;
		for (size_t j = i, n = 1; j < word.size() && n <= args_->maxn; n++) {
			ngram.push_back(word[j++]);
			while (j < word.size() && (word[j] & 0xC0) == 0x80) {
				ngram.push_back(word[j++]);
			}
			if (n >= args_->minn && !(n == 1 && (i == 0 || j == word.size()))) {
				int32_t h = findFeature(ngram);
				if (h >= 0)ngrams.push_back(words_.m_size + h);
			}
		}
	}
}

/**
* @Function: ntokens count.
*/
int64_t Dictionary::ntokens() const {
	return ntokens_;
}

/**
* @Function: Ngrams initial.
*/
void Dictionary::initNgrams() {
	wordprops_.resize(words_.m_size);

	for (size_t i = 0; i < words_.m_size; i++) {
		wordprops_[i].word = words_.from_id(i);
		wordprops_[i].count = words_.m_id_to_freq[i];

		std::string word = BOW + wordprops_[i].word + EOW;
		wordprops_[i].subwords.clear();
		if (wordprops_[i].word != EOS) {
			computeSubwords(word, wordprops_[i].subwords);
		}
	}
}

/**
* @Function: TableDiscard initial.
*/
void Dictionary::initTableDiscard() {
	pdiscard_.resize(words_.m_size);
	for (size_t i = 0; i < words_.m_size; i++) {
		real f = real(wordprops_[i].count) / real(ntokens_);
		pdiscard_[i] = std::sqrt(args_->t / f) + args_->t / f;
	}
}

/**
* @Function: None.
*/
bool Dictionary::discard(int32_t id, real rand) const {
	assert(id >= 0);
	assert(id < words_.m_size);

	return rand > pdiscard_[id];
}

/**
* @Function: read word.
*/
bool Dictionary::readWord(std::istream& in, std::string& word) const {
	char c;
	std::streambuf& sb = *in.rdbuf();
	word.clear();
	while ((c = sb.sbumpc()) != EOF) {
		if (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
			c == '\f' || c == '\0') {
			if (word.empty()) {
				if (c == '\n') {
					word += EOS;
					return true;
				}
				continue;
			}
			else {
				if (c == '\n')
					sb.sungetc();
				return true;
			}
		}
		word.push_back(c);
	}
	// trigger eofbit
	in.get();
	return !word.empty();
}

/**
* @Function: read file.
*/
void Dictionary::readFromFile(std::istream& in) {
	std::string word;
	ntokens_ = 0;
	while (readWord(in, word)) {
		addWord(word);
		ntokens_++;
		if (words_.m_size % 1000000 == 0 && args_->verbose > 1) {
			std::cerr << "\rRead " << words_.m_size / 1000000 << "M words" << std::flush;
		}
	}
	words_.prune(args_->minCount);

	initFeature();
	initTargets();
	initNgrams();
	initTableDiscard();

	if (args_->verbose > 0) {
		std::cerr << "\rRead " << words_.m_size / 1000000 << "M words" << std::endl;
		std::cerr << "Number of words:  " << words_.m_size << std::endl;
		std::cerr << "Number of features: " << features_.m_size << std::endl;
		std::cerr << "Number of targets: " << targets_.m_size << std::endl;
	}
	if (words_.m_size == 0) {
		throw std::invalid_argument("Empty vocabulary. Check the input file Or Try a smaller -minCount value.");
	}
}

/**
* @Function: reset input iostream.
*/
void Dictionary::reset(std::istream& in) const {
	if (in.eof()) {
		in.clear();
		in.seekg(std::streampos(0));
	}
}

/**
* @Function: getCounts.
*/
std::vector<int64_t> Dictionary::getCounts() const {
	std::vector<int64_t> counts;
	for (int i = 0; i < wordprops_.size(); i++) {
		counts.push_back(wordprops_[i].count);
	}
	return counts;
}

/**
* @Function: getLine.
*/
int32_t Dictionary::getLine(std::istream& in, std::vector<std::vector<int32_t> >& sourceTypes,
	std::vector<std::vector<int32_t> >& sources,
	std::vector<int32_t>& targets, std::minstd_rand& rng) const {
	std::uniform_real_distribution<> uniform(0, 1);
	std::string token;
	vector<string> words;
	int32_t ntokens = 0;

	reset(in);
	sourceTypes.clear();
	sources.clear();
	targets.clear();
	words.clear();
	while (readWord(in, token)) {
		if (token == EOS)
			break;
		words.push_back(token);
	}

	int word_num = words.size();
	int valid = 0;
	
	for (int i = 0; i < word_num; i++) {
		int32_t wid = findWord(words[i]);
		int32_t tid = findTarget(words[i]);
		ntokens++;
		if (wid < 0 || tid < 0 || discard(wid, uniform(rng)))
			continue;
		valid++;
		sourceTypes.push_back(std::vector<int32_t>());
		sources.push_back(std::vector<int32_t>());
		sourceTypes[valid - 1].push_back(0);
		sources[valid - 1].push_back(wid);
		targets.push_back(tid);

		if (args_->model == model_name::skipgram)
			continue;

		if (ntokens > MAX_LINE_SIZE)
			break;
	}
	return ntokens;
}