/**
 * Copyright (c) 2016-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#pragma once

#include <vector>
#include <random>
#include <utility>
#include <memory>

#include "args.h"
#include "matrix.h"
#include "real.h"

#include <iostream>
#include <assert.h>
#include <algorithm>
#include <stdexcept>

class Model {
  protected:
    std::shared_ptr<Matrix> wi_;
    std::shared_ptr<Matrix> wo_;
    std::shared_ptr<Args> args_;
    Vector hidden_;
    Vector output_;
    Vector grad_;
    int32_t hsz_;
    int32_t osz_;
    real loss_;
    int64_t nexamples_;
    std::vector<real> t_sigmoid_;
    std::vector<real> t_log_;
    // used for negative sampling:
    std::vector<int32_t> negatives_;
    size_t negpos;

    int32_t getNegative(int32_t target);
    void initSigmoid();
    void initLog();

    static const int32_t NEGATIVE_TABLE_SIZE = 10000000;

  public:
    Model(std::shared_ptr<Matrix>, std::shared_ptr<Matrix>,
          std::shared_ptr<Args>, int32_t);

    real binaryLogistic(int32_t, bool, real);
    real negativeSampling(int32_t, real);

    void update(const std::vector<int32_t>&, int32_t, real);
    void updatePara(const std::vector<int32_t>&, int32_t, real);
    void computeHidden(const std::vector<int32_t>&, Vector&) const;

    void setTargetCounts(const std::vector<int64_t>&);
    void initTableNegatives(const std::vector<int64_t>&);
    real getLoss() const;
    real sigmoid(real) const;
    real log(real) const;
    real std_log(real) const;

    std::minstd_rand rng;
};

constexpr int64_t SIGMOID_TABLE_SIZE = 512;
constexpr int64_t MAX_SIGMOID = 8;
constexpr int64_t LOG_TABLE_SIZE = 512;

Model::Model(
    std::shared_ptr<Matrix> wi,
    std::shared_ptr<Matrix> wo,
    std::shared_ptr<Args> args,
    int32_t seed)
    : hidden_(args->dim),
      output_(wo->size(0)),
      grad_(args->dim),
      rng(seed) {
    wi_ = wi;
    wo_ = wo;
    args_ = args;
    osz_ = wo->size(0);
    hsz_ = args->dim;
    negpos = 0;
    loss_ = 0.0;
    nexamples_ = 1;
    t_sigmoid_.reserve(SIGMOID_TABLE_SIZE + 1);
    t_log_.reserve(LOG_TABLE_SIZE + 1);
    initSigmoid();
    initLog();
}

real Model::binaryLogistic(int32_t target, bool label, real lr) {
    real score = sigmoid(wo_->dotRow(hidden_.data_, target));
    real alpha = lr * (real(label) - score);
    grad_.addRow(*wo_, target, alpha);
    wo_->addRow(hidden_.data_, target, alpha);
    if (label) {
        return -log(score);
    } else {
        return -log(1.0 - score);
    }
}

real Model::negativeSampling(int32_t target, real lr) {
    real loss = 0.0;
    grad_.zero();
    for (int32_t n = 0; n <= args_->neg; n++) {
        if (n == 0) {
            loss += binaryLogistic(target, true, lr);
        } else {
            loss += binaryLogistic(getNegative(target), false, lr);
        }
    }
    return loss;
}

void Model::computeHidden(const std::vector<int32_t>& input, Vector& hidden) const {
    assert(hidden.size() == hsz_);
    hidden.zero();
    for (auto it = input.cbegin(); it != input.cend(); ++it) {
        hidden.addRow(*wi_, *it);
    }
    hidden.mul(1.0 / input.size());
}

void Model::update(const std::vector<int32_t>& input, int32_t target, real lr) {
    assert(target >= 0);
    assert(target < osz_);
    if (input.size() == 0) return;
    computeHidden(input, hidden_);
    if (args_->loss == loss_name::ns) {
        loss_ += negativeSampling(target, lr);
    } else {
        //std::cout << "error loss"
    }
    nexamples_ += 1;

    for (auto it = input.cbegin(); it != input.cend(); ++it) {
        wi_->addRow(grad_.data_, *it, 1.0);
    }
}

void Model::updatePara(const std::vector<int32_t>& input, int32_t target, real lr) {
    vector<int32_t> source;
    source.push_back(input[0]);
    update(source, target, lr);

    int input_size = input.size();
    if (input_size > 1) {
        vector<int32_t> feature;
        for (int i = 1; i < input_size; i++) {
            feature.push_back(input[i]);
        }
        update(feature, target, lr);
        nexamples_ -= 1;
    }
}

void Model::setTargetCounts(const std::vector<int64_t>& counts) {
    assert(counts.size() == osz_);
    if (args_->loss == loss_name::ns) {
        initTableNegatives(counts);
    }
}

void Model::initTableNegatives(const std::vector<int64_t>& counts) {
    real z = 0.0;
    for (size_t i = 0; i < counts.size(); i++) {
        z += pow(counts[i], 0.5);
    }
    for (size_t i = 0; i < counts.size(); i++) {
        real c = pow(counts[i], 0.5);
        for (size_t j = 0; j < c * NEGATIVE_TABLE_SIZE / z; j++) {
            negatives_.push_back(i);
        }
    }
    std::shuffle(negatives_.begin(), negatives_.end(), rng);
}

int32_t Model::getNegative(int32_t target) {
    int32_t negative;
    do {
        negative = negatives_[negpos];
        negpos = (negpos + 1) % negatives_.size();
    } while (target == negative);
    return negative;
}

real Model::getLoss() const {
    return loss_ / nexamples_;
}

void Model::initSigmoid() {
    for (int i = 0; i < SIGMOID_TABLE_SIZE + 1; i++) {
        real x = real(i * 2 * MAX_SIGMOID) / SIGMOID_TABLE_SIZE - MAX_SIGMOID;
        t_sigmoid_.push_back(1.0 / (1.0 + std::exp(-x)));
    }
}

void Model::initLog() {
    for (int i = 0; i < LOG_TABLE_SIZE + 1; i++) {
        real x = (real(i) + 1e-5) / LOG_TABLE_SIZE;
        t_log_.push_back(std::log(x));
    }
}

real Model::log(real x) const {
    if (x > 1.0) {
        return 0.0;
    }
    int64_t i = int64_t(x * LOG_TABLE_SIZE);
    return t_log_[i];
}

real Model::std_log(real x) const {
    return std::log(x+1e-5);
}

real Model::sigmoid(real x) const {
    if (x < -MAX_SIGMOID) {
        return 0.0;
    } else if (x > MAX_SIGMOID) {
        return 1.0;
    } else {
        int64_t i = int64_t((x + MAX_SIGMOID) * SIGMOID_TABLE_SIZE / MAX_SIGMOID / 2);
        return t_sigmoid_[i];
    }
}