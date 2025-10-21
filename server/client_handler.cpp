#include "client_handler.h"

#include <iostream>
#include <utility>

#include "../common/constants.h"

ClientHandler::ClientHandler(Socket&& skt_client, size_t id,
                               Queue<ClientAction>& actiones_clients):
        protocol(ProtocolServer(std::move(skt_client))),
        id(id),
        mensajes_a_enviar{},
        recv(protocol, id, actiones_clients),
        send(protocol, id, mensajes_a_enviar) {}

ClientHandler::~ClientHandler() {
    hard_kill();
    mensajes_a_enviar.close();
    recv.join();
    send.join();
}

void ClientHandler::ejecutar() {
    recv.start();
    send.start();
}

bool ClientHandler::is_alive() {
    if (!recv.is_alive() || !send.is_alive()) {
        return false;
    }

    return true;
}

void ClientHandler::hard_kill() {
    recv.stop();
    send.stop();
    if (!protocol.canal_recibir_cerrado()) {
        protocol.shutdown(SHUT_BOTH_CLOSED);
    }
}

size_t ClientHandler::get_id() { return id; }

void ClientHandler::server_enviar(uint16_t cantidad_nitros_activos, uint8_t mensaje) {
    server_msg msg{cantidad_nitros_activos, mensaje};
    mensajes_a_enviar.try_push(std::move(msg));
}
