#include "client_handler.h"

#include <iostream>
#include <utility>

#include "../common/constants.h"
#include "../common/dto/server_msg.h"

ClientHandler::ClientHandler(Socket&& skt_client, size_t id, Queue<ClientAction>& actiones_clients):
        protocol(ServerProtocol(std::move(skt_client))),
        id(id),
        mensajes_a_enviar{},
        recv(protocol, id, actiones_clients),
    send(protocol, id, mensajes_a_enviar),
    recv_started(false),
    send_started(false) {}

ClientHandler::~ClientHandler() {
    hard_kill();
    mensajes_a_enviar.close();
    if (recv_started) {
        try { recv.join(); } catch (...) {}
    }
    if (send_started) {
        try { send.join(); } catch (...) {}
    }
}

void ClientHandler::ejecutar() {
    start_recv_only();
    start_send_only();
}

void ClientHandler::start_recv_only() {
    if (!recv_started) {
        recv.start();
        recv_started = true;
    }
}

void ClientHandler::start_send_only() {
    if (!send_started) {
        send.start();
        send_started = true;
    }
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

void ClientHandler::server_enviar_pos(uint32_t pid, int16_t x, int16_t y, float angle) {
    ServerOutMsg out{};
    out.type = ServerOutType::Pos;
    out.id = pid;
    out.x = x;
    out.y = y;
    out.angle = angle;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_positions_to_all(const std::vector<PlayerPos>& positions) {
    for (const auto& pp : positions) {
        server_enviar_pos(pp.id, pp.x, pp.y, pp.angle);
    }
}

void ClientHandler::send_rooms_to_client(const std::vector<RoomInfo>& rooms) {
    ServerOutMsg out{};
    out.type = ServerOutType::Rooms;
    out.rooms = rooms;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_ok_to_client() {
    ServerOutMsg out{};
    out.type = ServerOutType::Ok;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_your_id_to_client(uint32_t pid) {
    ServerOutMsg out{};
    out.type = ServerOutType::YourId;
    out.your_id = pid;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_player_name_to_client(uint32_t pid, const std::string& username) {
    ServerOutMsg out{};
    out.type = ServerOutType::PlayerName;
    out.id = pid;
    out.username = username;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_room_created_to_client(uint8_t room_id) {
    ServerOutMsg out{};
    out.type = ServerOutType::RoomCreated;
    out.room_id = room_id;
    mensajes_a_enviar.try_push(std::move(out));
}
