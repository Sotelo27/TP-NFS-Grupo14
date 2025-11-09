#include "client_list.h"

#include <vector>
#include <iostream>  // AGREGAR ESTE INCLUDE

#include "../common/constants.h"
#include "../common/player_aux.h"

void ClientListProtected::agregar_client(std::shared_ptr<ClientHandler> client) {
    std::lock_guard<std::mutex> lock(m);
    clients.push_back(std::move(client));
}

void ClientListProtected::reap(std::vector<size_t>& clients_eliminados) {
    std::lock_guard<std::mutex> lock(m);

    clients.remove_if([&clients_eliminados](const std::shared_ptr<ClientHandler>& c) {
        if (c && c->is_alive()) {
            return false;
        }

        if (c) {
            size_t id = c->get_id();
            c->hard_kill();
            clients_eliminados.push_back(id);
        }
        return true;
    });
}

void ClientListProtected::clear(std::vector<size_t>& clients_eliminados) {
    std::lock_guard<std::mutex> lock(m);

    for (auto& c: clients) {
        if (!c)
            continue;
        size_t id = c->get_id();
        clients_eliminados.push_back(id);
        c->hard_kill();
    }

    clients.clear();
}

void ClientListProtected::send_pos_to(size_t id, int16_t x, int16_t y, float angle) {
    std::lock_guard<std::mutex> lock(m);

    for (auto& c : clients) {
        if (c->get_id() == id) {
            c->server_enviar_pos(id, x, y, angle);
            break;
        }
    }
}

void ClientListProtected::broadcast_player_positions(const std::vector<PlayerPos>& positions) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client: clients) {
        if (client && client->is_alive()) {
            client->send_positions_to_all(positions);
        }
    }
}

void ClientListProtected::broadcast_players_list(const std::vector<PlayerInfo>& players) {
    std::lock_guard<std::mutex> lock(m);
    std::cout << "[ClientList] Broadcasting players list to " << clients.size() << " clients\n";
    for (auto& client: clients) {
        if (client && client->is_alive()) {
            client->send_players_list_to_client(players);
        }
    }
}

void ClientListProtected::broadcast_map_info(const std::vector<PlayerTickInfo>& players,
                                             const std::vector<NpcTickInfo>& npcs,
                                             const std::vector<EventInfo>& events) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client: clients) {
        if (client && client->is_alive()) {
            client->send_map_info_to_client(players, npcs, events);
        }
    }
}

ClientListProtected::~ClientListProtected() {
    for (auto& c: clients) {
        c->hard_kill();
    }

    clients.clear();
}
