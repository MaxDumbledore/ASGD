//
// Created by max on 2021/4/23.
//

#include "Client.h"
#include "Constants.h"
#include "Utils.h"

Client::Client(const Dataset& trainSet,
               const TrainSampler& sampler,
               asio::io_context& ioContext)
    : id(-1),
      trainLoader(torch::data::make_data_loader<Dataset, TrainSampler>(
          trainSet,
          sampler,
          TRAIN_BATCH_SIZE)),
      iter(0, trainLoader->begin()),
      sslContext(asio::ssl::context::sslv23) {
    builder.setData(model.getNoGradParams());
    for (auto& i : model.getNoGradParams())
        dims.emplace_back(i.sizes().vec());
    sslContext.use_certificate_chain_file(CERT_PATH + "client.pem");
    sslContext.use_private_key_file(CERT_PATH + "client.pem",
                                    asio::ssl::context::pem);
    sslContext.load_verify_file(CERT_PATH + "ca.pem");
    socket = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket>>(
        ioContext, sslContext);
    socket->set_verify_mode(asio::ssl::verify_peer);
}

void Client::iterateOneBatch() {
    lastParams = model.getNoGradParams();
    model.train(iter);
    nextIter();
}

void Client::nextIter() {
    if (++iter.second == trainLoader->end()) {
        iter.second = trainLoader->begin();
        ++iter.first;
    }
}

std::vector<at::Tensor> Client::getCurrentUpdate() const {
    auto params = model.getNoGradParams();
    std::vector<at::Tensor> result(params.size());
    for (int i = 0; i < result.size(); i++)
        result[i] = params[i] - lastParams[i];
    return result;
}

void stepDebug(const std::string &func, const asio::error_code& err) {
    if (err)
        std::cerr << func + " failed: " << err.message() << std::endl;
    else
        std::clog << func + " succeeded!" << std::endl;
}

void Client::connect(
    const asio::ip::basic_resolver<asio::ip::tcp,
                                   asio::any_io_executor>::results_type&
        endpoints) {
    asio::error_code err;
    asio::connect(socket->lowest_layer(), endpoints, err);
    stepDebug(__func__, err);
}

void Client::start() {
    if (handshake() && receiveIdAndInitialParams()) {
        iterateOneBatch();
        sendUpdate();
    }
    socket->shutdown();
}

bool Client::handshake() {
    asio::error_code err;
    socket->handshake(asio::ssl::stream_base::client, err);
    stepDebug(__func__, err);
    return !err.operator bool();
}

bool Client::receiveIdAndInitialParams() {
    asio::error_code err;
    buf.resize(4 + builder.size() * 4);
    asio::read(*socket, asio::buffer(buf), err);
    stepDebug(__func__, err);
    id = bytesToInt(buf.substr(0, 4));
    builder.setData(streamToFloatVec(buf,builder.size(),4));
    model.setParams(builder.getData(dims));
    return !err.operator bool();
}

bool Client::sendUpdate() {
    asio::error_code err;
    builder.setData(getCurrentUpdate());
    buf=floatVecToStream(builder.getData());
    asio::write(*socket, asio::buffer(buf), err);
    stepDebug(__func__, err);
    return !err.operator bool();
}