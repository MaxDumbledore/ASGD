//
// Created by 40461 on 2021/4/29.
//

#include "GlobalParams.h"

void GlobalParams::update(const std::vector<at::Tensor> &delta) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    for (int i = 0; i < delta.size(); i++)
        params[i] += delta[i];
}

std::vector<at::Tensor> GlobalParams::getParams() const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    return params;
}

void GlobalParams::setParams(std::vector<at::Tensor> &&_params) {
    std::unique_lock<std::shared_mutex> lock(mutex);
    n = 0;
    for (auto &i:_params) {
        i.set_requires_grad(false);
        n += i.numel();
    }
    params = std::move(_params);
}

int GlobalParams::length() {
    return n;
}
