//
// Created by max on 2021/4/23.
//

#include "Session.h"
#include "SessionManager.h"
#include <iostream>

using namespace std;

Session::Session(int clientID, asio::ssl::stream<asio::ip::tcp::socket> socket, SessionManager &manager) :
        clientID(clientID), socket(std::move(socket)), manager(manager) {

}

void Session::start() {
    handshake();
}

void Session::handshake() {
    socket.async_handshake(asio::ssl::stream_base::server,
                           [this](const std::error_code &error) {
                               if (!error)
                                   manager.stop(shared_from_this());
                               else
                                   cerr << error.message() << endl;
                           });
}

void Session::stop() {
    socket.shutdown();
}
