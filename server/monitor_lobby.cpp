#include "monitor_lobby.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "map/map_config_loader.h"

#ifndef COLLISION_PATH
#define COLLISION_PATH "."
#endif

#define RUTA_MAPA std::string(COLLISION_PATH) + "/CollisionTest2.yaml"

#ifndef ROOM_LEAVE
#define ROOM_LEAVE 0x04
#endif

MonitorLobby::MonitorLobby(float nitro_duracion): actions_in(), nitro_duracion(nitro_duracion) {
    init_dispatch();
}

void MonitorLobby::init_dispatch() {
    action_dispatch = {
        {ClientAction::Type::Room, [this](ClientAction act){ handle_room_action(std::move(act)); }},
        {ClientAction::Type::Name, [this](ClientAction act){ handle_name_action(std::move(act)); }},
        {ClientAction::Type::Move, [this](ClientAction act){ handle_move_action(std::move(act)); }},
        {ClientAction::Type::StartGame, [this](ClientAction act){ handle_start_game(std::move(act)); }}
    };
}

void MonitorLobby::handle_room_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    if (act.room_cmd == ROOM_CREATE) {
        std::cout << "[Lobby] Processing ROOM_CREATE from conn_id=" << act.id << "\n";

        {
            std::optional<uint8_t> old_rid;
            auto handler = detach_from_current_room_locked(act.id, &old_rid);
            if (handler) {
                pending.emplace(act.id, handler);
                std::cout << "[Lobby] Moved conn_id=" << act.id << " from room to pending\n";
                if (old_rid.has_value()) {
                    std::cout << "[Lobby] Broadcasting updated players list for old room_id=" << (int)old_rid.value() << "\n";
                    broadcast_players_in_room_locked(old_rid.value());
                }
            }
        }

        uint8_t rid = create_room_locked(/*max_players=*/8, /*creator_conn_id=*/act.id);

        auto itp = pending.find(act.id);
        if (itp != pending.end() && itp->second) {
            std::cout << "[Lobby] Sending ROOM_CREATED(" << (int)rid << ") to conn_id=" << act.id << "\n";
            itp->second->send_room_created_to_client(rid);
        } else {
            std::cout << "[Lobby] ERROR: No se encontró pending conn_id=" << act.id << "\n";
        }

        std::cout << "[Lobby] Broadcasting updated room list\n";
        broadcast_rooms_to_pending_locked();

        std::cout << "[Lobby] Auto-joining creator conn_id=" << act.id
                  << " into room_id=" << (int)rid << "\n";
        if (join_room_locked(act.id, rid)) {
            std::cout << "[Lobby] Auto-join successful\n";
            broadcast_rooms_to_pending_locked();
        } else {
            std::cout << "[Lobby] ERROR: Auto-join failed\n";
        }
    } else if (act.room_cmd == ROOM_JOIN) {
        std::cout << "[Lobby] JOIN request conn_id=" << act.id
                  << " -> room_id=" << (int)act.room_id << "\n";
        if (join_room_locked(act.id, act.room_id)) {
            broadcast_rooms_to_pending_locked();
        }
    } else if (act.room_cmd == ROOM_LEAVE) {
        std::cout << "[Lobby] Processing ROOM_LEAVE from conn_id=" << act.id << "\n";
        std::optional<uint8_t> old_rid;
        auto handler = detach_from_current_room_locked(act.id, &old_rid);
        if (handler) {
            pending.emplace(act.id, handler);
            std::cout << "[Lobby] Moved conn_id=" << act.id << " from room to pending\n";
            broadcast_rooms_to_pending_locked();
            if (old_rid.has_value()) {
                broadcast_players_in_room_locked(old_rid.value());
            }
        } else {
            std::cout << "[Lobby] ROOM_LEAVE ignored: conn_id=" << act.id << " had no room\n";
            broadcast_rooms_to_pending_locked();
        }
    } else {
        std::cout << "[Lobby] Unknown room_cmd=" << (int)act.room_cmd
                  << " from conn_id=" << act.id << "\n";
    }
}

void MonitorLobby::handle_name_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "[Lobby] NAME action from conn_id=" << act.id
              << ": '" << act.username << "'\n";
    auto itb = bindings.find(act.id);
    if (itb == bindings.end()) {
        pending_names[act.id] = std::move(act.username);
        std::cout << "[Lobby] Stored pending name for conn_id=" << act.id << "\n";
    } else {
        auto rid = itb->second.first;
        auto pid = itb->second.second;
        auto itr = rooms.find((uint8_t)rid);
        if (itr != rooms.end()) {
            std::cout << "[Lobby] Applying name to player_id=" << pid
                      << " in room_id=" << (int)rid
                      << ": '" << act.username << "'\n";
            itr->second.game.set_player_name(pid, std::move(act.username));
        }
    }
}

void MonitorLobby::handle_move_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    auto itb = bindings.find(act.id);
    if (itb == std::end(bindings)) {
        auto itp = pending.find(act.id);
        if (itp != pending.end() && itp->second) {
            itp->second->send_rooms_to_client(list_rooms_locked());
            std::cout << "[Lobby] MOVE from conn_id=" << act.id
                      << " ignored (not in room). Sent rooms list again.\n";
        }
        return;
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

void MonitorLobby::handle_start_game(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    
    std::cout << "[MonitorLobby] conn_id=" << act.id << " solicitó START_GAME\n";
    
    uint8_t target_room_id = 0;
    bool found = false;
    
    auto itb = bindings.find(act.id);
    if (itb != bindings.end()) {
        target_room_id = itb->second.first;
        found = true;
    }
    
    if (!found) {
        std::cout << "[MonitorLobby] ERROR: conn_id=" << act.id << " no está en ninguna sala\n";
        return;
    }
    
    auto itr = rooms.find(target_room_id);
    if (itr == rooms.end()) {
        std::cout << "[MonitorLobby] ERROR: sala " << (int)target_room_id << " no existe\n";
        return;
    }
    
    // NUEVO: Validar que sea el creador de la sala (admin permanente)
    if (act.id != itr->second.creator_conn_id) {
        std::cout << "[MonitorLobby] ERROR: conn_id=" << act.id 
                  << " no es el creador/admin (admin=" << itr->second.creator_conn_id << ")\n";
        return;
    }
    
    std::string map_name = act.races.empty() ? "LibertyCity" : act.races[0].first;
    std::cout << "[MonitorLobby] Admin (creador) inicia partida con mapa: " << map_name << "\n";
    
    std::vector<std::pair<int32_t, int32_t>> checkpoints;
    
    for (const auto& [conn, bind] : bindings) {
        if (bind.first == target_room_id) {
            try {
                itr->second.clients.get_handler_by_conn(conn)->send_race_start(map_name, checkpoints);
                std::cout << "[MonitorLobby] RaceStart enviado a conn_id=" << conn << "\n";
            } catch (const std::exception& e) {
                std::cerr << "[MonitorLobby] Error enviando RaceStart a conn_id=" << conn << ": " << e.what() << "\n";
            }
        }
    }
}

size_t MonitorLobby::reserve_connection_id() {
    std::lock_guard<std::mutex> lk(m);
    return next_conn_id++;
}

void MonitorLobby::add_pending_connection(std::shared_ptr<ClientHandler> ch, size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    pending.emplace(conn_id, std::move(ch));
    std::cout << "[Lobby] New pending connection conn_id=" << conn_id << "\n";
    
    auto it = pending.find(conn_id);
    if (it != pending.end() && it->second) {
        it->second->send_rooms_to_client(list_rooms_locked());
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
    
    for (const auto& [conn_id, bind] : bindings) {
        if (bind.first == room_id) {
            size_t player_id = bind.second;
            std::string username = itr->second.game.get_player_name(player_id);
            
            uint8_t health = 100;
            uint32_t race_time = 0;
            
            // NUEVO: determinar si es admin (es el creador)
            bool is_admin = (conn_id == itr->second.creator_conn_id);
            
            try {
                players_list.emplace_back(
                    static_cast<uint32_t>(player_id), 
                    username, 
                    false, // is_ready
                    is_admin,
                    health,
                    race_time
                );
            } catch (...) {
                players_list.emplace_back(
                    static_cast<uint32_t>(player_id), 
                    username, 
                    false,
                    is_admin,
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

uint8_t MonitorLobby::create_room_locked(uint8_t max_players, size_t creator_conn_id) {
    while (rooms.count(next_room_id)) ++next_room_id;
    uint8_t rid = next_room_id++;

    auto [it, _] = rooms.try_emplace(rid, rid, nitro_duracion, max_players, creator_conn_id);
    std::cout << "[Lobby] Created room id=" << (int)rid 
              << ", max_players=" << (int)max_players 
              << ", creator_conn_id=" << creator_conn_id << "\n";

    try {
        MapConfig cfg = MapConfigLoader::load_tiled_file(RUTA_MAPA);
        it->second.game.load_map(cfg);
        std::cout << "[Lobby] Loaded map from YAML: " << RUTA_MAPA
              << " (rects=" << cfg.rects.size() << ", polylines=" << cfg.polylines.size()
              << ", spawns=" << cfg.spawns.size() << ")\n";
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

    uint8_t current = 0;
    for (const auto& [conn, bind]: bindings) {
        if (bind.first == room_id)
            current++;
    }
    if (current >= itr->second.max_players)
        return false;

    std::shared_ptr<ClientHandler> handler = itp->second;
    pending.erase(itp);

    size_t player_id = itr->second.game.add_player();
    bindings[conn_id] = std::make_pair(room_id, player_id);

    auto pn = pending_names.find(conn_id);
    if (pn != pending_names.end()) {
        std::cout << "[Lobby] Applying pending name for conn_id=" << conn_id << ": '" << pn->second << "'\n";
        itr->second.game.set_player_name(player_id, pn->second);
        if (handler) {
            handler->send_player_name_to_client(static_cast<uint32_t>(player_id), pn->second);
        }
        pending_names.erase(pn);
    }

    itr->second.clients.agregar_client(handler);
    handler->start_send_only();
    handler->send_your_id_to_client((uint32_t)(player_id));
    std::cout << "[Lobby] conn_id=" << conn_id << " joined room_id=" << (int)room_id
              << " as player_id=" << player_id << "\n";

    broadcast_players_in_room_locked(room_id);
    broadcast_rooms_to_pending_locked();

    return true;
}

std::shared_ptr<ClientHandler> MonitorLobby::detach_from_current_room_locked(size_t conn_id, std::optional<uint8_t>* old_room) {
    auto itb = bindings.find(conn_id);
    if (itb == bindings.end()) return nullptr;

    uint8_t rid = itb->second.first;
    size_t pid = itb->second.second;
    auto itr = rooms.find(rid);
    if (itr == rooms.end()) {
        bindings.erase(itb);
        return nullptr;
    }

    if (old_room) *old_room = rid;

    std::shared_ptr<ClientHandler> handler = itr->second.clients.remover_por_conn_id(conn_id);
    if (!handler) {
        bindings.erase(conn_id);
        return nullptr;
    }

    try {
        std::string username = itr->second.game.get_player_name(pid);
        if (!username.empty()) {
            pending_names[conn_id] = username;
        }
    } catch (...) {
    }

    try {
        itr->second.game.remove_player(pid);
        std::cout << "[Lobby] Removed player_id=" << pid << " from room_id=" << (int)rid << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[Lobby] Error removing player_id=" << pid << ": " << e.what() << "\n";
    }

    bindings.erase(conn_id);

    size_t still_in_room = 0;
    for (const auto& [c, bind] : bindings) {
        if (bind.first == rid) still_in_room++;
    }

    if (still_in_room == 0) {
        std::cout << "[Lobby] Room " << (int)rid << " is now empty. Stopping loop and erasing room.\n";
        stop_room_loop_locked(itr->second);
        rooms.erase(itr);
        broadcast_rooms_to_pending_locked();
    } else {
        broadcast_players_in_room_locked(rid);
        broadcast_rooms_to_pending_locked();
    }

    return handler;
}

void MonitorLobby::reap_locked() {
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
        
        if (room_changed) {
            broadcast_players_in_room_locked(pr.room_id);
            broadcast_rooms_to_pending_locked();
        }

        bool empty_room = true;
        for (const auto& [conn, bind]: bindings) {
            if (bind.first == pr.room_id) {
                empty_room = false;
                break;
            }
        }
        if (empty_room) {
            std::cout << "[Lobby] Reap erasing empty room_id=" << (int)pr.room_id << "\n";
            stop_room_loop_locked(pr);
            it = rooms.erase(it);
            broadcast_rooms_to_pending_locked();
        } else {
            ++it;
        }
    }

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
            ClientAction act;
            bool processed_any = false;

            while (actions_in.try_pop(act)) {
                processed_any = true;
                std::cout << "[Lobby] ======================================\n";
                std::cout << "[Lobby] Acción recibida: type=" << (int)act.type
                          << ", id=" << act.id << "\n";

                auto it = action_dispatch.find(act.type);
                if (it != action_dispatch.end()) {
                    it->second(act);
                } else {
                    std::cout << "[Lobby] Acción desconocida ignorada\n";
                }

                std::cout << "[Lobby] ======================================\n";
            }

            {
                std::lock_guard<std::mutex> lk(m);
                reap_locked();
            }

            iteration_count++;
            if (iteration_count % 100 == 0) {
                std::lock_guard<std::mutex> lk(m);
                std::cout << "[Lobby] Status: " << rooms.size()
                          << " rooms, " << pending.size()
                          << " pending connections\n";
            }

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

MonitorLobby::~MonitorLobby() {
    try {
        stop();
    } catch (...) {
    }
    try {
        Thread::join();
    } catch (...) {
    }
}
