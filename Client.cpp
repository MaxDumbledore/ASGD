//
// Created by max on 2021/4/23.
//

#include "Client.h"
#include <random>
#include "Constants.h"
#include "Utils.h"
//#define DEBUG

// If the macro DEBUG is defined, the message of intermediate process will be
// print.

Client::Client(const Dataset& trainSet, asio::io_context& ioContext)
    : trainSet(trainSet),
      id(-1),
      sslContext(asio::ssl::context::sslv23),
      iter(-1, nullptr) {
    helper.setData(model.getNoGradParams());
    for (auto& i : model.getNoGradParams())
        dims.emplace_back(i.sizes().vec());

    // load certifications and construct socket.

    sslContext.use_certificate_chain_file(CERT_PATH + "client.pem");
    sslContext.use_private_key_file(CERT_PATH + "client.pem",
                                    asio::ssl::context::pem);
    sslContext.load_verify_file(CERT_PATH + "ca.pem");
    socket = std::make_unique<asio::ssl::stream<asio::ip::tcp::socket>>(
        ioContext, sslContext);
    socket->set_verify_mode(asio::ssl::verify_peer);
}

/**
 * @brief make the member trainLoader
 * Using DistributedRandomSampler to divide dataset into equal parts and sample
 * a batch randomly every time.
 * @return true if no error occur.
 * @return false if id is not correct.
 */

bool Client::makeTrainLoader() {
    if (id < 0 || id >= PARTICIPATE_COUNT)
        return false;
    trainLoader = torch::data::make_data_loader<Dataset, TrainSampler>(
        trainSet, TrainSampler(trainSet.size().value(), PARTICIPATE_COUNT, id),
        TRAIN_BATCH_SIZE);
    iter = {0, trainLoader->begin()};
    return true;
}

bool Client::iterateOneBatch() {
    if (finished())
        return false;
    lastParams = model.getNoGradParams();  // save the former parameters for
                                           // calculating the update value.
    model.train(iter);

    //    std::clog << __func__ << ":Epoch " << iter.first << std::endl;

    nextIter();
    return true;
}

void Client::nextIter() {
    if (++iter.second == trainLoader->end()) {
        iter.second = trainLoader->begin();
        ++iter.first;
    }
}

bool Client::finished() const {
    return iter.first == EPOCHS;
}

std::vector<at::Tensor> Client::getCurrentUpdate() const {
    auto params = model.getNoGradParams();
    std::vector<at::Tensor> result(params.size());
    for (int i = 0; i < result.size(); i++)
        result[i] = params[i] - lastParams[i];
    return result;
}

/**
 * @brief a debug function for printing message of intermediate process.
 *
 * @param func
 * @param err
 */
void stepDebug(const std::string& func, const asio::error_code& err) {
#ifdef DEBUG
    if (err)
        std::cerr << func + " failed: " << err.message() << std::endl;
    else
        std::clog << func + " succeeded!" << std::endl;
#endif
}

void Client::connect(
    const asio::ip::basic_resolver<asio::ip::tcp,
                                   asio::any_io_executor>::results_type&
        endpoints) {
    asio::error_code err;
    asio::connect(socket->lowest_layer(), endpoints, err);
    stepDebug(__func__, err);
}

#define RANDOM_DELAY

// If the macro RANDOM_DELAY is defined then every time before training the
// process waits for a random time interval. This is for estimulating the
// real network environment.

void Client::start(Dataset* testSet) {
    clock_t start = clock(), g = 0;
    if (handshake() && receiveIdAndInitialParams()) {
#ifdef RANDOM_DELAY
        static std::random_device rd;
        static std::mt19937 e(rd());
        std::uniform_int_distribution<int> dist(0, 100);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(dist(e)));  //休眠100毫秒
#endif
        int cnt = 0;
        do {
            if (++cnt % 50 == 0) {
                // std::clog << "------" << cnt << "------" << std::endl;
                if (testSet) {
                    auto d = clock();
                    //  std::clog << "Correctness: " << test(*testSet) <<
                    //  std::endl;
                    g += (clock() - d);
                }
                std::clog /* << "TimeCost: "*/
                    << (double)(clock() - start - g) / CLOCKS_PER_SEC
                    << std::endl;
            }
        } while (iterateOneBatch() && sendUpdate() && receiveParams());
    }
    socket->shutdown();
}

double Client::test(const Dataset& testSet) {
    TestLoader testLoader(
        torch::data::make_data_loader(testSet, TEST_BATCH_SIZE));
    return (double)model.matchCount(testLoader) / testSet.size().value();
}

bool Client::handshake() {
    asio::error_code err;
    socket->handshake(asio::ssl::stream_base::client, err);
    stepDebug(__func__, err);
    return !err.operator bool();
}

bool Client::receiveIdAndInitialParams() {
    asio::error_code err;
    buf.resize(4 + helper.size() * 4);
    asio::read(*socket, asio::buffer(buf), err);
    stepDebug(__func__, err);
    id = bytesToInt(buf.substr(0, 4));
    makeTrainLoader();
    helper.setData(streamToFloatVec(buf, helper.size(), 4));
    model.setParams(helper.getData(dims));
    return !err.operator bool();
}

bool Client::sendUpdate() {
    asio::error_code err;
    helper.setData(getCurrentUpdate());
    buf = floatVecToStream(helper.getData()) + (finished() ? 'S' : 'C');
    asio::write(*socket, asio::buffer(buf), err);
    stepDebug(__func__, err);
    return !err.operator bool();
}

bool Client::receiveParams() {
    asio::error_code err;
    buf.resize(helper.size() * 4);
    asio::read(*socket, asio::buffer(buf), err);
    stepDebug(__func__, err);
    helper.setData(streamToFloatVec(buf, helper.size()));
    model.setParams(helper.getData(dims));
    return !err.operator bool();
}

const Params& Client::getHelper() const {
    return helper;
}