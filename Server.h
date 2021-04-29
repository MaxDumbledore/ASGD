//
// Created by max on 2021/4/23.
//

#ifndef ASGD_SERVER_H
#define ASGD_SERVER_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include "SessionManager.h"

class Server {
public:
    Server(asio::io_context &ioContext, uint16_t port);

private:
    asio::ip::tcp::acceptor tcpAcceptor;
    asio::ssl::context sslContext;
    SessionManager sessionManager;

    void doAccept();
};


#endif //ASGD_SERVER_H
