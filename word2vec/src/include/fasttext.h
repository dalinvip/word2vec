/**
* @Author: bamtercelboo
* @Date£º 2018/03/15
* @File: fasttext.h
* @Contact£º bamtercelboo@{gmail.com, 163.com}
* @Function£ºNone
*/


#pragma once

#include<time.h>

#include<iostream>
#include<vector>
#include<string>
#include<memory>
#include<atomic>
#include<iomanip>

#include "args.h"
#include "dictionary.h"
#include "matrix.h"
#include "model.h"
#include "real.h"
#include "utils.h"

class Fasttext {
protected:
	std::shared_ptr<Args> args_;
	std::shared_ptr<Dictionary> dict_;

	std::shared_ptr<Matrix> input_;
	std::shared_ptr<Matrix> output_;

	std::shared_ptr<Model> model_;

	std::atomic<int64_t> tokenCount;
	std::atomic<real> loss_;

	clock_t start_;

	void startThreads();

public:
	Fasttext();
	void saveVectors();
	void printInfo(real, real, std::ostream&);

	void skipgram(Model&, real, const std::vector<std::vector<int32_t> >&, const std::vector<int32_t>&);
	void trainThread(int32_t);
	void train(const Args);
};

Fasttext::Fasttext() {}

void Fasttext::train(const Args args) {
	args_ = std::make_shared<Args>(args);
	dict_ = std::make_shared<Dictionary>(args_);
	if (args_->input == "-") {
		//manage expectations
		throw std::invalid_argument("Cannot use  stdin for training");
	}
	std::ifstream ifs(args_->input);
	if (!ifs.is_open()) {
		throw std::invalid_argument(args_->input + "cannot be opened for training!");
	}

	dict_->readFromFile(ifs);
	ifs.close();
}










