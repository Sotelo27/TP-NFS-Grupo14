#include "server.h"

#define DURACION_NITRO 3
constexpr std::string_view CERRAR_SERVER = "q";

Server::Server(const char* puerto)
    : lobby(DURACION_NITRO), acceptor(puerto, lobby) {}

void Server::start() {
    lobby.start();
    acceptor.start();

    std::string entrada;
    while (std::getline(std::cin, entrada)) {
        std::cout << "[Server] stdin line: '" << entrada << "'" << std::endl;
        if (entrada == CERRAR_SERVER) {
            std::cout << "[Server] Recibi comando de cierre 'q', saliendo del loop de entrada" << std::endl;
            break;
        }
    }
}

Server::~Server() {
    acceptor.stop_acceptor();
    lobby.stop();

    acceptor.join();
    lobby.join();
}
