#include "room_manager.h"
#include <iostream>

RoomManager::RoomManager(float nitro_duracion) : rooms(), next_room_id(1), nitro_duracion(nitro_duracion) {}

uint8_t RoomManager::create_room(uint8_t max_players, size_t creator_conn_id) {
    std::lock_guard<std::mutex> lk(m);
    while (rooms.count(next_room_id)) ++next_room_id;
    uint8_t rid = next_room_id++;
    auto [it, _] = rooms.try_emplace(rid, rid, nitro_duracion, max_players, creator_conn_id);
    it->second.started = false;
    std::cout << "[RoomManager] Created room id=" << (int)rid << ", max_players=" << (int)max_players << ", creator=" << creator_conn_id << "\n";
    start_room_loop_locked(it->second);
    return rid;
}

std::vector<RoomInfo> RoomManager::list_rooms() const {
    std::lock_guard<std::mutex> lk(m);
    std::vector<RoomInfo> out;
    out.reserve(rooms.size());

    for (const auto& [rid, partida] : rooms) {
        if (partida.started)
            continue;

        out.emplace_back(RoomInfo{rid, 0, partida.max_players});
    }

    return out;
}

std::vector<RoomInfo> RoomManager::list_rooms_with_counts(const BindingManager& bindings) const {
    std::lock_guard<std::mutex> lk(m);
    std::vector<RoomInfo> out;
    out.reserve(rooms.size());

    for (const auto& [rid, partida] : rooms) {
        if (partida.started)
            continue;

        // Count bound connections in this room
        size_t current = bindings.count_in_room(rid);
        out.emplace_back(RoomInfo{rid, static_cast<uint8_t>(current), partida.max_players});
    }

    return out;
}


bool RoomManager::join_room_from_pending(size_t conn_id, uint8_t room_id, PendingManager& pending, BindingManager& bindings) {
    std::lock_guard<std::mutex> lk(m);
    auto pit = rooms.find(room_id);
    if (pit == rooms.end()) return false;
    Match& room = pit->second;
    if (room.started) return false;

    // check capacity via bindings count
    size_t current = bindings.count_in_room(room_id);
    if (current >= room.max_players) return false;

    // get pending data
    auto handler = pending.take_pending(conn_id);
    if (!handler) return false;

    std::string username = "";
    auto pn = pending.take_pending_name(conn_id);
    if (pn.has_value()) username = std::move(pn.value());
    uint8_t car_id = 0;
    auto pc = pending.take_pending_car(conn_id);
    if (pc.has_value()) car_id = pc.value();

    try {
        size_t player_id = room.game.add_player(username, car_id);
        bindings.add_binding(conn_id, room_id, player_id);

        // attach handler into clients list
        room.clients.agregar_client(handler);
        handler->start_send_only();
        handler->send_your_id_to_client((uint32_t)player_id);
        if (!username.empty()) handler->send_player_name_to_client(static_cast<uint32_t>(player_id), username);

        std::cout << "[RoomManager] conn_id=" << conn_id << " joined room_id=" << (int)room_id
                  << " as player_id=" << player_id << ", username='" << username << "', car_id=" << (int)car_id << "\n";

        // broadcast updated players list
        std::vector<PlayerInfo> players;
        for (size_t conn : bindings.conns_in_room(room_id)) {
            auto b = bindings.find_binding(conn);
            if (!b.has_value()) continue;
            size_t pid = b->second;
            std::string uname = room.game.get_player_name(pid);
            bool is_admin = (conn == room.creator_conn_id);
            players.emplace_back(static_cast<uint32_t>(pid), uname, false, is_admin, 100u, 0u);
        }
        room.clients.broadcast_players_list(players);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[RoomManager] ERROR creating player for conn " << conn_id << ": " << e.what() << "\n";
        // restore handler to pending so client is not lost
        pending.add_pending(conn_id, handler);
        return false;
    }
}

std::shared_ptr<ClientHandler> RoomManager::detach_from_room(size_t conn_id, BindingManager& bindings, PendingManager& pending) {
    std::lock_guard<std::mutex> lk(m);
    auto b = bindings.find_binding(conn_id);
    if (!b.has_value()) return nullptr;

    uint8_t rid = b->first;
    size_t pid = b->second;
    auto itr = rooms.find(rid);
    if (itr == rooms.end()) {
        bindings.remove_binding(conn_id);
        return nullptr;
    }

    Match& room = itr->second;
    // remove handler from clients by conn id
    std::shared_ptr<ClientHandler> handler = room.clients.remover_por_conn_id(conn_id);
    if (!handler) {
        bindings.remove_binding(conn_id);
        return nullptr;
    }

    try {
        std::string username = room.game.get_player_name(pid);
        if (!username.empty()) {
            pending.store_pending_name(conn_id, username);
        }
    } catch (...) {}

    try {
        room.game.remove_player(pid);
        std::cout << "[RoomManager] Removed player_id=" << pid << " from room_id=" << (int)rid << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[RoomManager] Error removing player_id=" << pid << ": " << e.what() << "\n";
    }

    bindings.remove_binding(conn_id);

    // if room now empty -> stop and erase
    bool empty = true;
    for (const auto& kv : bindings.conns_in_room(rid)) {
        (void)kv;
        empty = false;
        break;
    }
    if (empty) {
        std::cout << "[RoomManager] Room " << (int)rid << " is now empty. Stopping loop and erasing room.\n";
        stop_room_loop_locked(room);
        rooms.erase(itr);
    } else {
        // broadcast players list
        std::vector<PlayerInfo> players;
        for (size_t conn : bindings.conns_in_room(rid)) {
            auto b2 = bindings.find_binding(conn);
            if (!b2.has_value()) continue;
            size_t pid2 = b2->second;
            std::string uname = room.game.get_player_name(pid2);
            bool is_admin = (conn == room.creator_conn_id);
            players.emplace_back(static_cast<uint32_t>(pid2), uname, false, is_admin, 100u, 0u);
        }
        room.clients.broadcast_players_list(players);
    }

    return handler;
}

bool RoomManager::push_move_to_room(uint8_t room_id, size_t player_id, Movement movement) {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return false;
    ClientAction routed(static_cast<size_t>(player_id), movement);
    it->second.actions.push(routed);
    return true;
}

bool RoomManager::push_improvement_to_room(uint8_t room_id, size_t player_id, uint8_t improvement_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return false;
    ClientAction routed; // usar default y setear campos
    routed.type = ClientAction::Type::Improvement;
    routed.id = static_cast<size_t>(player_id);
    routed.improvement_id = improvement_id;
    it->second.actions.push(routed);
    return true;
}

bool RoomManager::push_cheat_to_room(uint8_t room_id, size_t player_id, uint8_t cheat_code) {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return false;
    ClientAction routed;
    routed.type = ClientAction::Type::Cheat;
    routed.id = player_id;
    routed.cheat = cheat_code;
    it->second.actions.push(routed);
    return true;
}

void RoomManager::reap(BindingManager& bindings, PendingManager& pending) {
    (void)pending;
    std::lock_guard<std::mutex> lk(m);
    for (auto it = rooms.begin(); it != rooms.end();) {
        Match& pr = it->second;
        std::vector<size_t> removed_conn_ids;
        pr.clients.reap(removed_conn_ids);

        bool room_changed = false;
        for (size_t conn_id: removed_conn_ids) {
            auto b = bindings.find_binding(conn_id);
            if (b.has_value()) {
                size_t player_id = b->second;
                try {
                    pr.game.remove_player(player_id);
                } catch (const std::exception& e) {
                    std::cerr << "Error removing player " << player_id << ": " << e.what() << "\n";
                }
                bindings.remove_binding(conn_id);
                room_changed = true;
            }
        }

        if (room_changed) {
            std::vector<PlayerInfo> players;
            for (size_t conn : bindings.conns_in_room(pr.room_id)) {
                auto b = bindings.find_binding(conn);
                if (!b.has_value()) continue;
                size_t pid = b->second;
                std::string uname = pr.game.get_player_name(pid);
                bool is_admin = (conn == pr.creator_conn_id);
                players.emplace_back(static_cast<uint32_t>(pid), uname, false, is_admin, 100u, 0u);
            }
            pr.clients.broadcast_players_list(players);
        }

        bool empty_room = true;
        for (size_t c : bindings.conns_in_room(pr.room_id)) {
            (void)c; empty_room = false; break;
        }
        if (empty_room) {
            std::cout << "[RoomManager] Reap erasing empty room_id=" << (int)pr.room_id << "\n";
            stop_room_loop_locked(pr);
            it = rooms.erase(it);
        } else {
            ++it;
        }
    }
}

void RoomManager::stop_all_rooms() {
    std::lock_guard<std::mutex> lk(m);
    for (auto& kv : rooms) {
        stop_room_loop_locked(kv.second);
    }
    rooms.clear();
}

size_t RoomManager::room_count() const {
    std::lock_guard<std::mutex> lk(m);
    return rooms.size();
}

bool RoomManager::is_creator(size_t conn_id, uint8_t room_id) const {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return false;
    return it->second.creator_conn_id == conn_id;
}

void RoomManager::start_room_game(uint8_t room_id, const ClientAction& startAction, BindingManager& bindings, PendingManager& pending) {
    (void)pending;
    std::lock_guard<std::mutex> lk(m);
    auto itr = rooms.find(room_id);
    if (itr == rooms.end()) {
        std::cout << "[RoomManager] ERROR: room not found " << (int)room_id << "\n";
        return;
    }
    Match& room = itr->second;
    room.started = true;
    std::cout << "[RoomManager] Sala " << (int)room_id << " marcada como iniciada\n";

    std::string map_name = startAction.races.empty() ? "LibertyCity" : startAction.races[0].first;
    std::cout << "[RoomManager] Admin inicia partida con mapa: " << map_name << "\n";

    try {
        room.game.load_map_by_id(map_name);
        std::cout << "[RoomManager] Loaded map: " << map_name << "\n";
        room.game.start_current_race();
        std::cout << "[RoomManager] Started current race with map: " << map_name << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[RoomManager] Error loading map '" << map_name << "': " << e.what() << "\n";
    }

    uint8_t map_id = 0;
    if (map_name == "LibertyCity") map_id = 0;
    else if (map_name == "SanAndreas") map_id = 1;
    else if (map_name == "ViceCity") map_id = 2;

    // notify connected clients in that room via bindings
    for (size_t conn : bindings.conns_in_room(room_id)) {
        try {
            auto handler = room.clients.get_handler_by_conn(conn);
            if (handler) {
                std::vector<std::pair<int32_t,int32_t>> checkpoints; // empty for now
                handler->send_race_start(map_id, checkpoints);
                std::cout << "[RoomManager] RaceStart enviado a conn_id=" << conn << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "[RoomManager] Error enviando RaceStart a conn_id: " << e.what() << "\n";
        }
    }
}

void RoomManager::apply_player_name(uint8_t room_id, size_t player_id, std::string name) {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return;
    it->second.game.set_player_name(player_id, std::move(name));
}

void RoomManager::stop_room_loop_locked(Match& p) {
    if (p.loop.has_value()) {
        p.loop->stop();
        p.loop->join();
        p.loop.reset();
    }
}

void RoomManager::start_room_loop_locked(Match& p) {
    if (!p.loop.has_value()) {
        p.loop.emplace(p.game, p.clients, p.actions);
        p.loop->start();
    }
}

void RoomManager::stop_room_loop_and_erase(uint8_t room_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return;
    stop_room_loop_locked(it->second);
    rooms.erase(it);
}

void RoomManager::set_player_infinite_life(uint8_t room_id, size_t player_id, bool enable) {
    std::lock_guard<std::mutex> lk(m);
    auto it = rooms.find(room_id);
    if (it == rooms.end()) return;
    it->second.game.set_player_infinite_life(player_id, enable);
}
