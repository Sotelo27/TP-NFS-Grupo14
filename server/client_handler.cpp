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
    std::cout << "[ClientHandler] Queueing ROOMS message for conn_id=" << id << " (count=" << rooms.size() << ")\n";
    ServerOutMsg out{};
    out.type = ServerOutType::Rooms;
    out.rooms = rooms;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_ok_to_client() {
    std::cout << "[ClientHandler] Queueing OK message for conn_id=" << id << "\n";
    ServerOutMsg out{};
    out.type = ServerOutType::Ok;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_your_id_to_client(uint32_t pid) {
    std::cout << "[ClientHandler] Queueing YOUR_ID message for conn_id=" << id << " (player_id=" << pid << ")\n";
    ServerOutMsg out{};
    out.type = ServerOutType::YourId;
    out.your_id = pid;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_player_name_to_client(uint32_t pid, const std::string& username) {
    std::cout << "[ClientHandler] Queueing PLAYER_NAME message for conn_id=" << id << " (player_id=" << pid << ", name='" << username << "')\n";
    ServerOutMsg out{};
    out.type = ServerOutType::PlayerName;
    out.id = pid;
    out.username = username;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_room_created_to_client(uint8_t room_id) {
    std::cout << "[ClientHandler] Queueing ROOM_CREATED message for conn_id=" << id << " (room_id=" << (int)room_id << ")\n";
    ServerOutMsg out{};
    out.type = ServerOutType::RoomCreated;
    out.room_id = room_id;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_players_list_to_client(const std::vector<PlayerInfo>& players) {
    std::cout << "[ClientHandler] Queueing PLAYERS_LIST message for conn_id=" << id 
              << " (count=" << players.size() << ")\n";
    ServerOutMsg out{};
    out.type = ServerOutType::PlayersList;
    out.players = players;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_map_info_to_client(const std::vector<PlayerTickInfo>& players,
                                            const std::vector<NpcTickInfo>& npcs,
                                            const std::vector<EventInfo>& events,
                                            TimeTickInfo time_info) {
    //std::cout << "[ClientHandler] Queueing MAP_INFO message for conn_id=" << id  << " (players=" << players.size() << ")\n";
    ServerOutMsg out{};
    out.type = ServerOutType::MapInfo;
    out.players_tick = players;
    out.npcs_tick = npcs;
    out.events_tick = events;
    out.race_time = time_info;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_race_start(uint8_t map_id, const std::vector<std::pair<int32_t, int32_t>>& checkpoints, uint32_t tiempo_partida) {
    std::cout << "[ClientHandler] Queueing RACE_START for conn_id=" << id << " map_id=" << (int)map_id << "\n";
    ServerOutMsg out{};
    out.type = ServerOutType::RaceStart;
    out.map_id = map_id;
    out.checkpoints = checkpoints;
    out.tiempo_partida = tiempo_partida;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_results_to_client(const std::vector<PlayerResultCurrent>& current) {
    std::cout << "[ClientHandler] Queueing RESULTS for conn_id=" << id << " players=" << current.size() << "\n";
    ServerOutMsg out{};
    out.type = ServerOutType::Results;
    out.results_current = current;
    mensajes_a_enviar.try_push(std::move(out));
}

void ClientHandler::send_improvement_ok_to_client(uint32_t player_id, uint8_t improvement_id, bool success, uint32_t total_penalty_seconds) {
    std::cout << "[ClientHandler] Queueing IMPROVEMENT for conn_id=" << id
              << " player_id=" << player_id
              << " improvement=" << (int)improvement_id
              << " success=" << (success?1:0)
              << " total_penalty_seconds=" << total_penalty_seconds << "\n";
    ServerOutMsg out{};
    out.type = ServerOutType::ImprovementOk;
    out.id = player_id;
    out.improvement_id = improvement_id;
    out.improvement_success = success?1:0;
    out.total_penalty_seconds = total_penalty_seconds;
    mensajes_a_enviar.try_push(std::move(out));
}

