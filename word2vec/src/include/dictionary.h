/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
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
};

class Dictionary {
  protected:
	static const int32_t MAX_VOCAB_SIZE = 100000000;
	static const int32_t MAX_LINE_SIZE = 1024;

	int32_t findWord(const std::string&) const;
	void addWord(const std::string&);
	int32_t findTarget(const std::string&) const;
	void addTarget(const std::string&, int64_t);

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
	int64_t ntokens() const;
	int32_t getWordId(const std::string&) const;
	int32_t getTargetId(const std::string&) const;
	std::string getWord(int32_t) const;
	std::string getTarget(int32_t) const;

	std::vector<int64_t> getCounts() const;

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
* @Function: target initial.
*/
void Dictionary::initTargets() {
	//same as source
	for (size_t i = 0; i < words_.m_size; i++) {
		addTarget(words_.from_id(i), words_.m_id_to_freq[i]);
	}
}

/**
* @Function: Ngrams initial.
*/
void Dictionary::initNgrams() {
	wordprops_.resize(words_.m_size);

	for (size_t i = 0; i < words_.m_size; i++) {
		wordprops_[i].word = words_.from_id(i);
		wordprops_[i].count = words_.m_id_to_freq[i];
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