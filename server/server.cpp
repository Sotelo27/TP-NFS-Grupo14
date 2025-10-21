#include "server.h"

#define DURACION_NITRO 3

constexpr std::string_view CERRAR_SERVER = "q";

Server::Server(const char* puerto):
        game(DURACION_NITRO),
        clients(),
        actiones_clients(),
        acceptor(puerto, game, clients, actiones_clients),
        gameloop(game, clients, actiones_clients) {}


void Server::start() {
    acceptor.start();
    gameloop.start();

    std::string entrada;
    while (std::getline(std::cin, entrada)) {
        if (entrada == CERRAR_SERVER) {
            break;
        }
    }
}

Server::~Server() {
    acceptor.stop_acceptor();
    gameloop.stop();
    acceptor.join();
    gameloop.join();
}
