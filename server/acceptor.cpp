#include "acceptor.h"

#include <utility>

#include "../common/constants.h"

Acceptor::Acceptor(const char* puerto, MonitorLobby& lobby)
    : skt_server(Socket(puerto)), lobby(lobby) {}

void Acceptor::run() {
    while (should_keep_running()) {
        try {
            Socket skt_client = skt_server.accept();
            if (!should_keep_running()) {
                break;
            }

            size_t conn_id = lobby.reserve_connection_id();
            auto handler = std::make_shared<ClientHandler>(std::move(skt_client), conn_id, lobby.incoming_actions());
            // No iniciar hilos aún: se iniciarán al unirse a una sala
            lobby.add_pending_connection(std::move(handler), conn_id);
        } catch (const std::exception& e) {
            if (!should_keep_running()) break;
            std::cerr << "Something went wrong and an exception was caught: " << e.what() << "\n";
        }
    }
}

void Acceptor::stop_acceptor() {
    this->stop();
    skt_server.shutdown(SHUT_BOTH_CLOSED);
}

Acceptor::~Acceptor() {
    // MonitorLobby se encarga de limpiar
}
Acceptor::~Acceptor() {
    // MonitorLobby limpia pendientes/salas
}
