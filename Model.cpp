//
// Created by max on 2021/4/23.
//

#include "Model.h"
#include "Constants.h"

/**
 * @brief get a copy of parameters which requires_grad=False
 *
 * @return std::vector<at::Tensor>
 */
std::vector<at::Tensor> Model::getNoGradParams() const {
    std::vector<at::Tensor> result;

    // It has to be careful when copy a tensor.
    // First using detach() to get a detached tensor out of the training
    // graph(still share inner data) Then using clone() to get a deep copy of
    // the detached tensor. more to see: https://zhuanlan.zhihu.com/p/148061684

    for (auto& i : net.parameters())
        result.emplace_back(i.detach().clone());
    return result;
}

/**
 * @brief set parameters with our own data.
 *
 * @param _params
 */

void Model::setParams(const std::vector<at::Tensor>& _params) {
    torch::NoGradGuard noGrad;  // It's necessary to define a NoGradGuard
                                // because copy_() will change training graph!
    int cur = 0;
    for (auto& m : net.parameters())
        m.copy_(_params[cur++]);
}

/**
 * @brief train on batch which the iter points to.
 *
 * @param iter
 */

void Model::train(std::pair<int, torch::data::Iterator<Batch>> iter) {
    net.train();
    auto &data = iter.second->data, &target = iter.second->target;
    auto output = net.forward(data);
    auto loss = torch::nn::functional::cross_entropy(output, target);
    loss.backward();
    optimizer.step();
    optimizer.zero_grad();
}

Model::Model()
    : optimizer(net.parameters(),
                torch::optim::SGDOptions(LEARNING_RATE).momentum(MOMENTUM)) {}

/**
 * @brief try to predict on the test dataset, and get the number of matched
 * labels.
 *
 * @param testLoader
 * @return int
 */

int Model::matchCount(const TestLoader& testLoader) {
    torch::NoGradGuard noGrad;
    net.eval();
    int correct = 0;
    for (const auto& batch : *testLoader) {
        auto data = batch.data, target = batch.target;
        auto output = net.forward(data);
        correct +=
            torch::argmax(output, 1).eq(target).sum().template item<int>();
    }
    return correct;
}
