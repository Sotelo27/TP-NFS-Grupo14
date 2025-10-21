#include "client_list.h"

#include <vector>

#include "../common/constants.h"

void ClientListProtected::agregar_client(std::unique_ptr<ClientHandler> client) {
    std::lock_guard<std::mutex> lock(m);

    clients.push_back(std::move(client));
}

void ClientListProtected::reap(std::vector<size_t>& clients_eliminados) {
    std::lock_guard<std::mutex> lock(m);

    clients.remove_if([&clients_eliminados](const std::unique_ptr<ClientHandler>& c) {
        if (c->is_alive()) {
            return false;
        }

        size_t id = c->get_id();
        c->hard_kill();
        clients_eliminados.push_back(id);
        return true;
    });
}

void ClientListProtected::clear(std::vector<size_t>& clients_eliminados) {
    std::lock_guard<std::mutex> lock(m);

    for (auto& c: clients) {
        size_t id = c->get_id();
        clients_eliminados.push_back(id);
        c->hard_kill();
    }

    clients.clear();
}

void ClientListProtected::broadcast_nitro_activado(uint16_t cantidad_nitros_activos) {
    std::lock_guard<std::mutex> lock(m);

    for (auto& c: clients) {
        c->server_enviar(cantidad_nitros_activos, CODE_NITRO_ACTIVATED);
    }
}

void ClientListProtected::broadcast_nitro_desactivado(uint16_t cantidad_nitros_activos) {
    std::lock_guard<std::mutex> lock(m);

    for (auto& c: clients) {
        c->server_enviar(cantidad_nitros_activos, CODE_NITRO_EXPIRED);
    }
}

ClientListProtected::~ClientListProtected() {
    for (auto& c: clients) {
        c->hard_kill();
    }

    clients.clear();
}
