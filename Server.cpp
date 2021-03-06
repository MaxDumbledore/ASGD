#include "Server.h"
#include <iostream>
#include "Constants.h"

using namespace std;

Server::Server(asio::io_context& ioContext, uint16_t port)
    : tcpAcceptor(ioContext,
                  asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      sslContext(asio::ssl::context::sslv23) {
    // load SSL certifications
    sslContext.set_options(asio::ssl::context::default_workarounds |
                           asio::ssl::context::no_sslv2);
    sslContext.use_certificate_chain_file(CERT_PATH + "server.pem");
    sslContext.use_private_key_file(CERT_PATH + "server.pem",
                                    asio::ssl::context::pem);
    sslContext.set_verify_mode(asio::ssl::verify_peer |
                               asio::ssl::verify_fail_if_no_peer_cert);
    sslContext.load_verify_file(CERT_PATH + "ca.pem");

    // First: accept all connections from participants.

    doAccept();
    ioContext.run();
    std::clog << "Accept finished!" << std::endl;

    // Second: start the training process.

    ioContext.restart();
    sessionManager.startAll();
    ioContext.run();
    std::clog << "Session finished!" << std::endl;

    // Third: As training finished, send back the final parameters.

    ioContext.restart();
    sessionManager.finishAll();
    ioContext.run();
    std::clog << "Send Result finished!" << std::endl;

    // for(int i=0;i<100;i++)
    // std::clog<<sessionManager.params().getData()[i]<<std::endl;
}

void Server::doAccept() {
    for (int curId = 0; curId < PARTICIPATE_COUNT; curId++) {
        tcpAcceptor.async_accept([this, curId](const asio::error_code& err,
                                               asio::ip::tcp::socket socket) {
            if (!err) {
                sessionManager.insert(std::make_shared<Session>(
                    curId,
                    asio::ssl::stream<asio::ip::tcp::socket>(std::move(socket),
                                                             sslContext),
                    sessionManager));
            } else
                std::cerr << err.message() << std::endl;
        });
    }
}