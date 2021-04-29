//
// Created by max on 2021/4/23.
//

#ifndef ASGD_SESSION_H
#define ASGD_SESSION_H

#include <asio.hpp>
#include <asio/ssl.hpp>

class SessionManager;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(int clientID, asio::ssl::stream<asio::ip::tcp::socket> socket, SessionManager &manager);

    void start();

    void stop();

private:
    int clientID;
    asio::ssl::stream<asio::ip::tcp::socket> socket;
    SessionManager &manager;
    std::string buf;

    void handshake();

    void sendIdAndParams();
};

using SessionPtr = std::shared_ptr<Session>;

#endif //ASGD_SESSION_H
