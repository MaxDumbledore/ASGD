#include <iostream>
#include <asio.hpp>
#include "Server.h"
#include "Client.h"

int main(int argc, char *argv[]) {
    try {
        if (argc <= 1 || (strcmp(argv[1], "-c") && strcmp(argv[1], "-s"))) {
            std::cerr << argv[1];
            std::cerr << "Options: -s(server) or -c(client)" << std::endl;
            return 1;
        }
        asio::io_context ioContext;
        if (argv[1][1] == 'c') {
            if (argc != 4) {
                std::cerr << "Usage: -c <host> <port>\n";
                return 1;
            }
            asio::ip::tcp::resolver resolver(ioContext);
            auto endpoints = resolver.resolve(argv[2], argv[3]);
            new Client(ioContext, endpoints);
        } else {
            if (argc != 3) {
                std::cerr << "Usage: -s <port>\n";
                return 1;
            }
            new Server(ioContext, std::stoi(argv[2]));
        }
        ioContext.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
}
