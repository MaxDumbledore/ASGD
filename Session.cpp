//
// Created by max on 2021/4/23.
//

#include "Session.h"
#include "SessionManager.h"
#include "Utils.h"
#include <iostream>

using namespace std;

Session::Session(int clientID, asio::ssl::stream<asio::ip::tcp::socket> socket, SessionManager &manager) :
        clientID(clientID), socket(std::move(socket)), manager(manager) {

}

void Session::start() {
    handshake();
}

void Session::handshake() {
    auto self(shared_from_this());
    socket.async_handshake(asio::ssl::stream_base::server,
                           [this, self](const asio::error_code &error) {
                               if (!error) {
                                   sendIdAndParams();
                                   std::clog << "handshake succeeded!" << std::endl;
                               } else
                                   std::cerr << error.message() << std::endl;
                           });
}

void Session::stop() {
    socket.shutdown();
}

void Session::sendIdAndParams() {
    auto self(shared_from_this());
    buf = intToByte(clientId);
    buf.reserve(buf.size() + manager.params().length() * 4);
    for(auto f:manager.params().getParams())

    asio::async_write(socket, asio::buffer(buf),
                      [this, self](const asio::error_code &err, std::size_t) {

                      });
}
