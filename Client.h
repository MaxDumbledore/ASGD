//
// Created by max on 2021/4/23.
//

#ifndef ASGD_CLIENT_H
#define ASGD_CLIENT_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include "Model.h"

class Client {
public:
    Client(const Dataset &trainSet, const TrainSampler &sampler, asio::io_context &ioContext);

    void connect(const asio::ip::tcp::resolver::results_type &endpoints);

    void start();

private:
    int rank;
    TrainLoader trainLoader;
    std::pair<int, torch::data::Iterator<Batch>> iter;
    Model model;
    std::vector<at::Tensor> lastParams;

    asio::ssl::context sslContext;
    std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>> socket;
    std::string buf;

    void nextIter();

    void handshake();
};


#endif //ASGD_CLIENT_H
