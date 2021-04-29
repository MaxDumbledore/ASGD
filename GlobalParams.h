//
// Created by 40461 on 2021/4/29.
//

#ifndef ASGD_GLOBALPARAMS_H
#define ASGD_GLOBALPARAMS_H

#include <torch/torch.h>
#include <shared_mutex>

class GlobalParams {
public:
    GlobalParams() = default;

    void setParams(std::vector<at::Tensor> &&_params);

    void update(const std::vector<at::Tensor> &delta);

    std::vector<at::Tensor> getParams() const;

    int length();

private:
    mutable std::shared_mutex mutex;

    std::vector<at::Tensor> params;

    int n;
};

#endif //ASGD_GLOBALPARAMS_H
