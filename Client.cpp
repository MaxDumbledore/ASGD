//
// Created by max on 2021/4/23.
//

#include "Client.h"
#include "Constants.h"

Client::Client(const Dataset &trainSet, const TrainSampler &sampler, asio::io_context &ioContext) :
        rank(-1),
        trainLoader(torch::data::make_data_loader<Dataset, TrainSampler>(trainSet, sampler, TRAIN_BATCH_SIZE)),
        iter(0, trainLoader->begin()),
        sslContext(asio::ssl::context::sslv23) {
    sslContext.use_certificate_chain_file(CERT_PATH + "client.pem");
    sslContext.use_private_key_file(CERT_PATH + "client.pem", asio::ssl::context::pem);
    sslContext.load_verify_file(CERT_PATH + "ca.pem");
    socket = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket>>
            (ioContext, sslContext);
    socket->set_verify_mode(asio::ssl::verify_peer);
}

void Client::nextIter() {

}

void stepDebug(const std::string func, const asio::error_code &err) {
    if (err)
        std::cerr << func + " failed: " << err.message() << std::endl;
    else
        std::clog << func + " succeeded!" << std::endl;
}

void Client::connect(const asio::ip::basic_resolver<asio::ip::tcp, asio::any_io_executor>::results_type &endpoints) {
    asio::error_code err;
    asio::connect(socket->lowest_layer(), endpoints, err);
    stepDebug(__func__, err);
}

void Client::start() {
    handshake();
    socket->shutdown();
}

void Client::handshake() {
    asio::error_code err;
    socket->handshake(asio::ssl::stream_base::client, err);
    stepDebug(__func__, err);
}