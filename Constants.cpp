//
// Created by max on 2021/4/23.
//

#include "Constants.h"

const std::string DATA_PATH = "/home/max/MNIST_DATA";  // Path of MNIST Dataset
const std::string CERT_PATH = "/home/max/ssl/";        // Path of TLS/SSL
                                                 // certification
const int PARTICIPATE_COUNT = 2;  // Number of participants, for convenience of
                                  // implementation, it's pre-defined.
const int TRAIN_BATCH_SIZE = 64;   // batch size for training
const int TEST_BATCH_SIZE = 1024;  // batch size for testing
const int EPOCHS = 1;
const double LEARNING_RATE = 0.01;
const double MOMENTUM = 0.5;