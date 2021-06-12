//
// Created by max on 2021/4/23.
//

#ifndef ASGD_MNISTCNN_H
#define ASGD_MNISTCNN_H


#include <torch/torch.h>

/**
 * @brief an implementation of the neural network for training
 * Refer to https://github.com/pytorch/examples/tree/master/cpp/mnist
 */

class MnistCNN : public torch::nn::Module {
public:

    MnistCNN();

    torch::Tensor forward(torch::Tensor x);

private:
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Dropout2d conv2Drop;
    torch::nn::Linear fc1;
    torch::nn::Linear fc2;
};


#endif //ASGD_MNISTCNN_H
