#include "client_list.h"

#include <vector>

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
        if (!client)
            continue;
        client->send_positions_to_all(positions);
    }
}

ClientListProtected::~ClientListProtected() {
    for (auto& c: clients) {
        c->hard_kill();
    }

    clients.clear();
}
