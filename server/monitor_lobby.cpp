#include "monitor_lobby.h"

#include <chrono>
#include <iostream>
#include <thread>

MonitorLobby::MonitorLobby(float nitro_duracion): actions_in(), nitro_duracion(nitro_duracion) {}

size_t MonitorLobby::reserve_connection_id() {
    std::lock_guard<std::mutex> lk(m);
    return next_conn_id++;
}

void MonitorLobby::add_pending_connection(std::shared_ptr<ClientHandler> ch, size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    pending.emplace(conn_id, std::move(ch));
    std::cout << "[Lobby] New pending connection conn_id=" << conn_id << "\n";
    // Enviar listado de salas actual al cliente que acaba de entrar
    auto it = pending.find(conn_id);
    if (it != pending.end() && it->second) {
        it->second->send_rooms_to_client(list_rooms_locked());
        // iniciar solo recepcion para procesar NAME/ROOM y luego de ahi entrar a sala
        // practicamente sera posponer el hilo de envio hasta que el cliente cree o entre a  una sala >:3
        it->second->start_recv_only();
    }
}

std::vector<RoomInfo> MonitorLobby::list_rooms_locked() const {
    std::vector<RoomInfo> out;
    out.reserve(rooms.size());
    for (const auto& [rid, partida]: rooms) {
        uint8_t current = 0;
        for (const auto& [conn, bind]: bindings) {
            if (bind.first == rid)
                current++;
        }
        out.push_back(RoomInfo{rid, current, partida.max_players});
    }
    return out;
}

void MonitorLobby::broadcast_rooms_to_pending_locked() {
    auto rooms_dto = list_rooms_locked();
    for (auto& kv: pending) {
        if (kv.second) {
            kv.second->send_rooms_to_client(rooms_dto);
        }
    }
}

void MonitorLobby::start_room_loop_locked(Partida& p) {
    if (!p.loop.has_value()) {
        p.loop.emplace(p.game, p.clients, p.actions);
        p.loop->start();
    }
}

void MonitorLobby::stop_room_loop_locked(Partida& p) {
    if (p.loop.has_value()) {
        p.loop->stop();
        p.loop->join();
        p.loop.reset();
    }
}

uint8_t MonitorLobby::create_room_locked(uint8_t max_players) {
    while (rooms.count(next_room_id)) ++next_room_id;
    uint8_t rid = next_room_id++;

    auto [it, _] = rooms.try_emplace(rid, rid, nitro_duracion, max_players);
    std::cout << "[Lobby] Created room id=" << (int)rid << ", max_players=" << (int)max_players << "\n";

    start_room_loop_locked(it->second);
    return rid;
}

bool MonitorLobby::join_room_locked(size_t conn_id, uint8_t room_id) {
    auto itp = pending.find(conn_id);
    if (itp == pending.end())
        return false;

    auto itr = rooms.find(room_id);
    if (itr == rooms.end())
        return false;

    // Chequear cupo
    uint8_t current = 0;
    for (const auto& [conn, bind]: bindings) {
        if (bind.first == room_id)
            current++;
    }
    if (current >= itr->second.max_players)
        return false;

    // Tomar el handler y quitarlo de 'pending'
    std::shared_ptr<ClientHandler> handler = itp->second;
    pending.erase(itp);

    size_t player_id = itr->second.game.add_player();
    bindings[conn_id] = std::make_pair(room_id, player_id);

    // Nombre pendiente -> setear en Game
    auto pn = pending_names.find(conn_id);
    if (pn != pending_names.end()) {
        std::cout << "[Lobby] Applying pending name for conn_id=" << conn_id << ": '" << pn->second << "'\n";
        itr->second.game.set_player_name(player_id, pn->second);
        // Enviar al cliente su username asociado a su player_id
        if (handler) {
            handler->send_player_name_to_client(static_cast<uint32_t>(player_id), pn->second);
        }
        pending_names.erase(pn);
    }

    // Agregar handler a la lista de clientes de la sala
    itr->second.clients.agregar_client(handler);

    // Arrancar hilo de envio ahora que esta en sala ya que recv en este caso ya estaria activo
    handler->start_send_only();

    // Aca se informa  al cliente su player_id para que renderice correctamente
    handler->send_your_id_to_client((uint32_t)(player_id));
    std::cout << "[Lobby] conn_id=" << conn_id << " joined room_id=" << (int)room_id
              << " as player_id=" << player_id << "\n";

    return true;
}

void MonitorLobby::reap_locked() {
    // Reap de cada sala
    for (auto it = rooms.begin(); it != rooms.end();) {
        auto& pr = it->second;
        std::vector<size_t> removed_conn_ids;
        pr.clients.reap(removed_conn_ids);
        for (size_t conn_id: removed_conn_ids) {
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
        for (const auto& [conn, bind]: bindings) {
            if (bind.first == pr.room_id) {
                empty_room = false;
                break;
            }
        }
        if (empty_room) {
            stop_room_loop_locked(pr);
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
                        // Unir automáticamente al creador
                        std::cout << "[Lobby] Auto-joining creator conn_id=" << act.id << " into room_id=" << (int)rid << "\n";
                        join_room_locked(act.id, rid);
                        broadcast_rooms_to_pending_locked();
                    } else if (act.room_cmd == ROOM_JOIN) {
                        std::cout << "[Lobby] JOIN request conn_id=" << act.id << " -> room_id=" << (int)act.room_id << "\n";
                        if (join_room_locked(act.id, act.room_id)) {
                            broadcast_rooms_to_pending_locked();
                        }
                    }
                } else if (act.type == ClientAction::Type::Name) {
                    std::lock_guard<std::mutex> lk(m);
                    auto itb = bindings.find(act.id);
                    if (itb == bindings.end()) {
                        pending_names[act.id] = act.username;
                        std::cout << "[Lobby] Stored pending name for conn_id=" << act.id << ": '" << act.username << "'\n";
                    } else {
                        auto rid = itb->second.first;
                        auto pid = itb->second.second;
                        auto itr = rooms.find((uint8_t)rid);
                        if (itr != rooms.end()) {
                            std::cout << "[Lobby] Applying name to player_id=" << pid << " in room_id=" << (int)rid << ": '" << act.username << "'\n";
                            itr->second.game.set_player_name(pid, std::move(act.username));
                        }
                    }
                } else if (act.type == ClientAction::Type::Move) {
                    std::lock_guard<std::mutex> lk(m);
                    auto itb = bindings.find(act.id);
                    if (itb == std::end(bindings)) {
                        // si no esta en sala, Vamos a ignorar todo lo demas coomo el MOVE y reenviar rooms
                        // solo a ese cliente para que elija
                        auto itp = pending.find(act.id);
                        if (itp != pending.end() && itp->second) {
                            itp->second->send_rooms_to_client(list_rooms_locked());
                            std::cout << "[Lobby] MOVE from conn_id=" << act.id << " ignored (not in room). Sent rooms list again.\n";
                        }
                        continue;
                    }
                    auto rid = itb->second.first;
                    auto pid = itb->second.second;
                    auto itr = rooms.find((uint8_t)rid);
                    if (itr != rooms.end()) {
                        ClientAction routed(pid, act.movement);
                        itr->second.actions.push(routed);
                        // Debug de ruteo: muestra de qué conexión y hacia qué sala/player se envió el movimiento
                        std::cout << "[Lobby] Routed MOVE from conn_id=" << act.id
                                  << " -> room_id=" << (int)rid
                                  << ", player_id=" << pid << "\n";
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

    // Apagar salas y limpiar pendientes
    {
        std::lock_guard<std::mutex> lk(m);
        for (auto& [rid, pr]: rooms) {
            stop_room_loop_locked(pr);
        }
        rooms.clear();
        pending.clear();
        bindings.clear();
        pending_names.clear();
    }
}
