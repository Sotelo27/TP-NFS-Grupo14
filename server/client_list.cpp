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
                                             const std::vector<EventInfo>& events,
                                             TimeTickInfo time_info) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client: clients) {
        if (client && client->is_alive()) {
            client->send_map_info_to_client(players, npcs, events, time_info);
        }
    }
}

void ClientListProtected::broadcast_results(const std::vector<PlayerResultCurrent>& current) {
    std::lock_guard<std::mutex> lock(m);
    std::cout << "[ClientList] Broadcasting RESULTS to " << clients.size() << " clients (n=" << current.size() << ")\n";
    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->send_results_to_client(current);
        }
    }
}

void ClientListProtected::broadcast_results_total(const std::vector<PlayerResultTotal>& total) {
    std::lock_guard<std::mutex> lock(m);
    std::cout << "[ClientList] Broadcasting RESULTS_TOTAL to " << clients.size() << " clients (n=" << total.size() << ")\n";
    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->send_results_total_to_client(total);
        }
    }
}

void ClientListProtected::broadcast_race_start(uint8_t map_id) {
    std::lock_guard<std::mutex> lock(m);
    std::cout << "[ClientList] Broadcasting RACE_START to " << clients.size() << " clients (map_id=" << (int)map_id << ")\n";
    std::vector<std::pair<int32_t,int32_t>> empty_checkpoints; // 0 porque esto se tiene que cambiar en un futuro
    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->send_race_start(map_id, empty_checkpoints);
        }
    }
}

void ClientListProtected::broadcast_market_time_info(TimeTickInfo time_info) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->send_market_time_to_client(time_info);
        }
    }
}

void ClientListProtected::broadcast_improvement_ok(uint32_t player_id, uint8_t improvement_id, bool success, uint32_t total_penalty_seconds) {
    std::lock_guard<std::mutex> lock(m);
    std::cout << "[ClientList] Broadcasting IMPROVEMENT_OK to " << clients.size()
              << " clients (player_id=" << player_id
              << ", improvement=" << (int)improvement_id
              << ", success=" << (success?1:0)
              << ", total_penalty_seconds=" << total_penalty_seconds << ")\n";
    for (auto& client : clients) {
        if (client && client->is_alive()) {
            client->send_improvement_ok_to_client(player_id, improvement_id, success, total_penalty_seconds);
        }
    }
}

ClientListProtected::~ClientListProtected() {
    for (auto& c: clients) {
        c->hard_kill();
    }

    clients.clear();
}

std::shared_ptr<ClientHandler> ClientListProtected::get_handler_by_conn(size_t conn_id) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& ch : clients) {
        if (ch && ch->get_id() == conn_id) {
            return ch;
        }
    }
    return nullptr;
}

std::shared_ptr<ClientHandler> ClientListProtected::remover_por_conn_id(size_t conn_id) {
    std::lock_guard<std::mutex> lock(m);
    
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (*it && (*it)->get_id() == conn_id) {
            std::shared_ptr<ClientHandler> handler = *it;
            clients.erase(it);
            std::cout << "[ClientList] Removed client conn_id=" << conn_id << " from list\n";
            return handler;
        }
    }
    
    std::cout << "[ClientList] Client conn_id=" << conn_id << " not found in list\n";
    return nullptr;
}
