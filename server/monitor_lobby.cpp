#include "monitor_lobby.h"

#include <iostream>

MonitorLobby::MonitorLobby(float nitro_duracion)
    : actions_in(), nitro_duracion(nitro_duracion) {}

size_t MonitorLobby::reserve_connection_id() {
    std::lock_guard<std::mutex> lk(m);
    return next_conn_id++;
}

void MonitorLobby::add_pending_connection(std::unique_ptr<ClientHandler> ch, size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    pending.emplace(conn_id, std::move(ch));
    // Enviar listado de salas actual al cliente que acaba de entrar
    auto it = pending.find(conn_id);
    if (it != pending.end() && it->second) {
        it->second->send_rooms_to_client(list_rooms_locked());
    }
}

std::vector<RoomInfo> MonitorLobby::list_rooms_locked() const {
    std::vector<RoomInfo> out;
    out.reserve(rooms.size());
    for (const auto& [rid, partida] : rooms) {
        // Estimación: cantidad de clientes activos en la sala
        uint8_t current = 0;
        // Aquí simplemente no contamos, o podríamos mantener un contador si hiciera falta.
        // Para mantenerlo simple, use el tamaño de bindings por room.
        for (const auto& [conn, bind] : bindings) {
            if (bind.first == rid) current++;
        }
        out.push_back(RoomInfo{rid, current, partida.max_players});
    }
    return out;
}

void MonitorLobby::broadcast_rooms_to_pending_locked() {
    auto rooms_dto = list_rooms_locked();
    for (auto& kv : pending) {
        if (kv.second) {
            kv.second->send_rooms_to_client(rooms_dto);
        }
    }
}

uint8_t MonitorLobby::create_room_locked(uint8_t max_players) {
    // Evitar colisión simple
    while (rooms.count(next_room_id)) ++next_room_id;
    uint8_t rid = next_room_id++;

    auto [it, _] = rooms.emplace(
        rid, Partida{rid, nitro_duracion, max_players}
    );
    // Crear y lanzar gameloop de la sala
    it->second.loop = std::make_unique<Gameloop>(it->second.game, it->second.clients, it->second.actions);
    it->second.loop->start();
    return rid;
}

bool MonitorLobby::join_room_locked(size_t conn_id, uint8_t room_id) {
    auto itp = pending.find(conn_id);
    if (itp == pending.end()) return false;

    auto itr = rooms.find(room_id);
    if (itr == rooms.end()) return false;

    // Chequear cupo
    uint8_t current = 0;
    for (const auto& [conn, bind] : bindings) {
        if (bind.first == room_id) current++;
    }
    if (current >= itr->second.max_players) return false;

    // Mover handler a la sala
    std::unique_ptr<ClientHandler> handler = std::move(itp->second);
    pending.erase(itp);
    size_t player_id = itr->second.game.add_player();
    bindings[conn_id] = std::make_pair(room_id, player_id);

    // Nombre pendiente -> setear en Game
    auto pn = pending_names.find(conn_id);
    if (pn != pending_names.end()) {
        itr->second.game.set_player_name(player_id, pn->second);
        pending_names.erase(pn);
    }

    // Agregar handler a la lista de clientes de la sala y arrancar (ya estaba ejecutando)
    itr->second.clients.agregar_client(std::move(handler));

    // Opcional: avisar OK al cliente
    // NOTA: no tenemos referencia directa tras mover. Para enviar algo inmediato,
    // hubiéramos enviado antes de mover. Lo omitimos.

    return true;
}

void MonitorLobby::reap_locked() {
    // Reap de cada sala: eliminar clientes muertos y remover jugadores del Game
    for (auto it = rooms.begin(); it != rooms.end();) {
        auto& pr = it->second;
        std::vector<size_t> removed_conn_ids;
        pr.clients.reap(removed_conn_ids);
        for (size_t conn_id : removed_conn_ids) {
            auto b = bindings.find(conn_id);
            if (b != bindings.end()) {
                size_t player_id = b->second.second;
                try {
                    pr.game.remove_player(player_id);
                } catch (const std::exception& e) {
                    std::cerr << "Error removing player " << player_id << ": " << e.what() << "\n";
                }
                bindings.erase(b);
            }
        }

        // Si sala queda vacía -> cerrar loop y borrar
        bool empty_room = true;
        for (const auto& [conn, bind] : bindings) {
            if (bind.first == pr.room_id) { empty_room = false; break; }
        }
        if (empty_room) {
            if (pr.loop) {
                pr.loop->stop();
                pr.loop->join();
            }
            it = rooms.erase(it);
        } else {
            ++it;
        }
    }

    // Limpiar pendientes muertos
    for (auto it = pending.begin(); it != pending.end();) {
        if (!it->second || !it->second->is_alive()) {
            it = pending.erase(it);
        } else {
            ++it;
        }
    }
}

void MonitorLobby::run() {
    while (should_keep_running()) {
        try {
            // Drenar cola global y enrutar
            ClientAction act;
            while (actions_in.try_pop(act)) {
                if (act.type == ClientAction::Type::Room) {
                    std::lock_guard<std::mutex> lk(m);
                    if (act.room_cmd == ROOM_CREATE) {
                        uint8_t rid = create_room_locked(/*max_players=*/8);
                        (void)rid;
                        broadcast_rooms_to_pending_locked();
                        // Unir automáticamente al cliente creador a su sala recién creada
                        join_room_locked(act.id, rid);
                        broadcast_rooms_to_pending_locked();
                    } else if (act.room_cmd == ROOM_JOIN) {
                        if (join_room_locked(act.id, act.room_id)) {
                            broadcast_rooms_to_pending_locked();
                        } else {
                            // TODO: opcional, enviar mensaje de error al cliente
                        }
                    }
                } else if (act.type == ClientAction::Type::Name) {
                    std::lock_guard<std::mutex> lk(m);
                    auto itb = bindings.find(act.id);
                    if (itb == bindings.end()) {
                        // Guardar para aplicar al unir
                        pending_names[act.id] = act.username;
                    } else {
                        auto rid = itb->second.first;
                        auto pid = itb->second.second;
                        auto itr = rooms.find((uint8_t)rid);
                        if (itr != rooms.end()) {
                            itr->second.game.set_player_name(pid, std::move(act.username));
                        }
                    }
                } else if (act.type == ClientAction::Type::Move) {
                    std::lock_guard<std::mutex> lk(m);
                    auto itb = bindings.find(act.id);
                    if (itb != bindings.end()) {
                        auto rid = itb->second.first;
                        auto pid = itb->second.second;
                        auto itr = rooms.find((uint8_t)rid);
                        if (itr != rooms.end()) {
                            // Reescribir id por id de Player y encolar en la sala
                            ClientAction routed(pid, act.movement);
                            itr->second.actions.push(routed);
                        }
                    } else {
                        // Ignorar movimientos si aún no eligió sala
                    }
                }
            }

            {
                std::lock_guard<std::mutex> lk(m);
                reap_locked();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } catch (const ClosedQueue&) {
            break;
        } catch (const std::exception& e) {
            std::cerr << "MonitorLobby error: " << e.what() << "\n";
        }
    }

    // Apagar salas
    {
        std::lock_guard<std::mutex> lk(m);
        for (auto& [rid, pr] : rooms) {
            if (pr.loop) { pr.loop->stop(); pr.loop->join(); }
        }
        rooms.clear();
        pending.clear();
        bindings.clear();
        pending_names.clear();
    }
}
