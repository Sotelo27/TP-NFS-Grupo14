#include "monitor_lobby.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "map/map_config_loader.h"

// COLLISION_PATH se define en CMakeLists.txt como macro del compilador
// Si no está definida, usar una ruta por defecto
#ifndef COLLISION_PATH
#define COLLISION_PATH "."
#endif

#define RUTA_MAPA std::string(COLLISION_PATH) + "/CollisionTest2.yaml" // Dejar vacío hasta tener un mapa válido

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
        
        // CRÍTICO: Iniciar AMBOS hilos para que el cliente pueda enviar Y recibir
        std::cout << "[Lobby] Starting recv and send threads for conn_id=" << conn_id << "\n";
        it->second->start_recv_only();
        it->second->start_send_only();
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

std::vector<PlayerInfo> MonitorLobby::get_players_in_room_locked(uint8_t room_id) const {
    std::vector<PlayerInfo> players_list;
    
    auto itr = rooms.find(room_id);
    if (itr == rooms.end()) {
        return players_list;
    }
    
    // Recolectar todos los jugadores en esta sala
    for (const auto& [conn_id, bind] : bindings) {
        if (bind.first == room_id) {
            size_t player_id = bind.second;
            std::string username = "Player_" + std::to_string(player_id);
            
            // Obtener vida y tiempo del Game si está disponible
            uint8_t health = 100;  // Valor por defecto
            uint32_t race_time = 0; // Valor por defecto
            
            try {
                // TODO: Cuando Game tenga métodos para obtener vida y tiempo, usarlos aquí
                // health = itr->second.game.get_player_health(player_id);
                // race_time = itr->second.game.get_player_race_time(player_id);
                
                players_list.emplace_back(
                    static_cast<uint32_t>(player_id), 
                    username, 
                    false, // is_ready
                    health,
                    race_time
                );
            } catch (...) {
                players_list.emplace_back(
                    static_cast<uint32_t>(player_id), 
                    username, 
                    false,
                    health,
                    race_time
                );
            }
        }
    }
    
    std::cout << "[Lobby] get_players_in_room_locked: room_id=" << (int)room_id 
              << ", found " << players_list.size() << " players\n";
    
    return players_list;
}

void MonitorLobby::broadcast_players_in_room_locked(uint8_t room_id) {
    auto itr = rooms.find(room_id);
    if (itr == rooms.end()) {
        std::cout << "[Lobby] broadcast_players_in_room_locked: room not found: " << (int)room_id << "\n";
        return;
    }
    
    std::vector<PlayerInfo> players_list = get_players_in_room_locked(room_id);
    
    std::cout << "[Lobby] Broadcasting " << players_list.size() 
              << " players to room_id=" << (int)room_id << "\n";
    
    // Broadcast a todos los clientes en esta sala
    itr->second.clients.broadcast_players_list(players_list);
}

void MonitorLobby::start_room_loop_locked(Match& p) {
    if (!p.loop.has_value()) {
        p.loop.emplace(p.game, p.clients, p.actions);
        p.loop->start();
    }
}

void MonitorLobby::stop_room_loop_locked(Match& p) {
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

    // TEMPORALMENTE comentado hasta tener una ruta de mapa válida
    
    try {
        MapConfig cfg = MapConfigLoader::load_tiled_file(RUTA_MAPA);
        it->second.game.load_map(cfg);
        std::cout << "[Lobby] Loaded map from YAML: " << RUTA_MAPA
              << " (rects=" << cfg.rects.size() << ", polylines=" << cfg.polylines.size()
              << ", spawns=" << cfg.spawns.size() << ", checkpoints=" << cfg.checkpoints.size() << ")\n";
    } catch (const std::exception& e) {
        std::cerr << "[Lobby] Failed to load map YAML '" << RUTA_MAPA
                  << "': " << e.what() << "\n";
    }

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

    // NUEVO: Broadcast lista actualizada de jugadores a todos en la sala
    broadcast_players_in_room_locked(room_id);

    return true;
}

void MonitorLobby::reap_locked() {
    // Reap de cada sala
    for (auto it = rooms.begin(); it != rooms.end();) {
        auto& pr = it->second;
        std::vector<size_t> removed_conn_ids;
        pr.clients.reap(removed_conn_ids);
        
        bool room_changed = false;
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
                room_changed = true;
            }
        }
        
        // Si hubo cambios, broadcast actualizado de jugadores
        if (room_changed) {
            broadcast_players_in_room_locked(pr.room_id);
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
    std::cout << "[Lobby] MonitorLobby iniciado, esperando acciones..." << std::endl;
    
    int iteration_count = 0;
    
    while (should_keep_running()) {
        try {
            // Drenar cola global y enrutar
            ClientAction act;
            bool processed_any = false;
            
            while (actions_in.try_pop(act)) {
                processed_any = true;
                std::cout << "[Lobby] ======================================\n";
                std::cout << "[Lobby] Acción recibida: type=" << (int)act.type 
                          << ", id=" << act.id << std::endl;
                
                if (act.type == ClientAction::Type::Room) {
                    std::lock_guard<std::mutex> lk(m);
                    if (act.room_cmd == ROOM_CREATE) {
                        std::cout << "[Lobby] Processing ROOM_CREATE from conn_id=" << act.id << "\n";
                        uint8_t rid = create_room_locked(/*max_players=*/8);
                        
                        // 1. Notificar al creador PRIMERO
                        auto itp = pending.find(act.id);
                        if (itp != pending.end() && itp->second) {
                            std::cout << "[Lobby] Sending ROOM_CREATED(" << (int)rid << ") to conn_id=" << act.id << "\n";
                            itp->second->send_room_created_to_client(rid);
                        } else {
                            std::cout << "[Lobby] ERROR: No se encontró pending conn_id=" << act.id << "\n";
                        }
                        
                        // 2. Broadcast de la nueva lista de salas
                        std::cout << "[Lobby] Broadcasting updated room list\n";
                        broadcast_rooms_to_pending_locked();
                        
                        // 3. Unir automáticamente al creador
                        std::cout << "[Lobby] Auto-joining creator conn_id=" << act.id << " into room_id=" << (int)rid << "\n";
                        if (join_room_locked(act.id, rid)) {
                            std::cout << "[Lobby] Auto-join successful\n";
                            // 4. Broadcast final después del join
                            broadcast_rooms_to_pending_locked();
                        } else {
                            std::cout << "[Lobby] ERROR: Auto-join failed\n";
                        }
                    } else if (act.room_cmd == ROOM_JOIN) {
                        std::cout << "[Lobby] JOIN request conn_id=" << act.id << " -> room_id=" << (int)act.room_id << "\n";
                        if (join_room_locked(act.id, act.room_id)) {
                            broadcast_rooms_to_pending_locked();
                        }
                    }
                } else if (act.type == ClientAction::Type::Name) {
                    std::lock_guard<std::mutex> lk(m);
                    std::cout << "[Lobby] NAME action from conn_id=" << act.id << ": '" << act.username << "'\n";
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
                        std::cout << "[Lobby] Routed MOVE from conn_id=" << act.id
                                  << " -> room_id=" << (int)rid
                                  << ", player_id=" << pid << "\n";
                    }
                }
                std::cout << "[Lobby] ======================================\n";
            }

            {
                std::lock_guard<std::mutex> lk(m);
                reap_locked();
            }

            // Log periódico cada 100 iteraciones para no saturar
            iteration_count++;
            if (iteration_count % 100 == 0) {
                std::lock_guard<std::mutex> lk(m);
                std::cout << "[Lobby] Status: " << rooms.size() << " rooms, " 
                          << pending.size() << " pending connections\n";
            }

            // Si no procesamos ninguna acción, dormir un poco más para no saturar CPU
            if (!processed_any) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
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
