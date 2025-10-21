#include "acceptor.h"

#include <utility>

#include "../common/constants.h"

Acceptor::Acceptor(const char* puerto, Game& game, ClientListProtected& clients,
                   Queue<ClientAction>& actions_clients):
        skt_server(Socket(puerto)),
        game(game),
        clients(clients),
        actions_clients(actions_clients) {}

void Acceptor::run() {
    while (should_keep_running()) {
        try {
            Socket skt_client = skt_server.accept();
            if (!should_keep_running()) {
                break;
            }

            size_t id = game.jugador_agregar();
            auto* c = new ClientHandler(std::move(skt_client), id, actions_clients);
            reap();

            clients.agregar_client(std::unique_ptr<ClientHandler>(c));
            c->ejecutar();
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Something went wrong and an exception was caught: " << e.what() << "\n";
        }
    }
}

void Acceptor::limpiar_jugadores(const std::vector<size_t>& ids) {
    for (size_t id: ids) {
        try {
            game.jugador_eliminar(id);
        } catch (const std::exception& e) {
            std::cerr << "Something went wrong and an exception was caught: " << e.what() << "\n";
        }
    }
}

void Acceptor::reap() {
    std::vector<size_t> ids;
    clients.reap(ids);
    limpiar_jugadores(ids);
}

void Acceptor::clear() {
    std::vector<size_t> ids;
    clients.clear(ids);
    limpiar_jugadores(ids);
}

void Acceptor::stop_acceptor() {
    this->stop();
    skt_server.shutdown(SHUT_BOTH_CLOSED);
}

Acceptor::~Acceptor() { clear(); }
