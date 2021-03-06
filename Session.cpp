//
// Created by max on 2021/4/23.
//

#include "Session.h"
#include <iostream>
#include "SessionManager.h"
#include "Utils.h"
//#define DEBUG
using namespace std;

// If the macro DEBUG is defined, the message of intermediate process will be
// print.

Session::Session(int clientId,
                 asio::ssl::stream<asio::ip::tcp::socket> socket,
                 SessionManager& manager)
    : clientId(clientId), socket(std::move(socket)), manager(manager) {}

void Session::stepDebug(const std::string& func, const asio::error_code& err) {
#ifdef DEBUG
    std::clog << "With Client " << clientId << ": " << func;
    if (err)
        std::clog << " failed because " << err.message() << std::endl;
    else
        std::clog << " succeeded!" << std::endl;
#endif
}

void Session::start() {
    handshake();
}

void Session::handshake() {
    string func = __func__;
    auto self(shared_from_this());
    socket.async_handshake(
        asio::ssl::stream_base::server,
        [this, self, func = std::move(func)](const asio::error_code& err) {
            stepDebug(func, err);
            if (!err)
                sendIdAndInitialParams();
        });
}

void Session::stop() {
    socket.shutdown();
}

/**
 * @brief give ID to every participant, and send initial parameters.
 *
 */

void Session::sendIdAndInitialParams() {
    string func = __func__;
    auto self(shared_from_this());
    buf = intToBytes(clientId) + floatVecToStream(manager.params().getData());
    asio::async_write(socket, asio::buffer(buf),
                      [this, self, func = std::move(func)](
                          const asio::error_code& err, std::size_t) {
                          stepDebug(func, err);
                          if (!err)
                              receiveUpdate();
                      });
}

/**
 * @brief recive updates and the state message.
 * if the state message is 'C', then the client has more batches to train, so
 * server continuely send parameters. if the state message is 'S', then the
 * client runs out of its batches, suspend this session.
 */

void Session::receiveUpdate() {
    string func = __func__;
    auto self(shared_from_this());
    buf.resize(manager.params().size() * 4 + 1);
    asio::async_read(
        socket, asio::buffer(buf),
        [this, self, func = std::move(func)](const asio::error_code& err,
                                             std::size_t) {
            stepDebug(func, err);
            if (err)
                return;
            asyncUpdate(streamToFloatVec(buf, manager.params().size()));
            if (buf.back() == 'C')
                sendParams();
        });
}

/**
 * @brief update the global parameters asynchronously
 * push the update function into the back of io_context object, so only after
 * other functions in io_context are completed, the update() can be implemented.
 * @param vec
 */

void Session::asyncUpdate(std::vector<float>&& vec) {
    string func = __func__;
    auto self(shared_from_this());
    // std::clog<<"---post---"<<clientId<<std::endl;
    asio::post(socket.get_executor(),
               [this, self, vec = std::move(vec), func = std::move(func)]() {
                   manager.params().update(vec);
                   stepDebug(func, asio::error_code());
               });
}

void Session::sendParams() {
    string func = __func__;
    auto self(shared_from_this());
    buf = floatVecToStream(manager.params().getData());
    asio::async_write(socket, asio::buffer(buf),
                      [this, self, func = std::move(func)](
                          const asio::error_code& err, std::size_t) {
                          stepDebug(func, err);
                          if (!err)
                              receiveUpdate();
                      });
}

void Session::sendFinalParams() {
    string func = __func__;
    stepDebug(func, asio::error_code());
    auto self(shared_from_this());
    buf = floatVecToStream(manager.params().getData());
    asio::async_write(socket, asio::buffer(buf),
                      [this, self, func = std::move(func)](
                          const asio::error_code& err, std::size_t) {
                          //   stepDebug(func, err);
                          manager.stop(self);
                      });
}