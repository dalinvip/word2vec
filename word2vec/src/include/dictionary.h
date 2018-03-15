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

	void initFeatures();
	void initTargets();

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

Dictionary::Dictionary(std::shared_ptr<Args> args) : args_(args) {
	words_.setCapacity(MAX_VOCAB_SIZE - 1);
	targets_.setCapacity(MAX_VOCAB_SIZE - 1);
}

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

	initFeatures();
	initTargets();

	initTableDiscard();
	if (args_->verbose > 0) {
		std::cerr << "\rRead " << words_.m_size / 1000000 << "M words" << std::endl;
		std::cerr << "Number of words:  " << words_.m_size << std::endl;
		std::cerr << "Number of features: " << features_.m_size << std::endl;
		std::cerr << "Number of targets: " << targets_.m_size << std::endl;
	}
	if (words_.m_size == 0) {
		throw std::invalid_argument(
			"Empty vocabulary. Try a smaller -minCount value.");
	}
}