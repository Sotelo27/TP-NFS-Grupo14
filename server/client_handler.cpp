#include "client_handler.h"

#include <iostream>
#include <utility>

#include "../common/constants.h"

ClientHandler::ClientHandler(Socket&& skt_client, size_t id,
                   Queue<ClientAction>& actiones_clients):
    protocol(ServerProtocol(std::move(skt_client))),
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
    // Enviar el id al cliente apenas inicia la sesión
    protocol.send_your_id((uint32_t)id);
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
    if (!protocol.is_recv_closed()) {
        protocol.shutdown(SHUT_BOTH_CLOSED);
    }
}

size_t ClientHandler::get_id() { return id; }

void ClientHandler::server_enviar_pos(uint32_t id, int16_t x, int16_t y, float angle) {
    server_msg_pos msg{};
    msg.id = id;
    msg.x = x;
    msg.y = y;
    msg.angle = angle;
    mensajes_a_enviar.try_push(std::move(msg));
}

void ClientHandler::send_positions_to_all(const std::vector<PlayerPos>& positions) {
    // Volver a enviar todas las posiciones; el cliente ya filtra por su id para cámara.
    for (const auto& pp : positions) {
        server_enviar_pos(pp.id, pp.x, pp.y, pp.angle);
    }
}

