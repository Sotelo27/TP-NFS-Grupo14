#include "monitor_lobby.h"

#include <chrono>
#include <iostream>
#include <thread>

#include "map/map_config_loader.h"

MonitorLobby::MonitorLobby(float nitro_duracion)
    : actions_in(), rooms(nitro_duracion), pending(), bindings(), nitro_duracion(nitro_duracion) {
    init_dispatch();
}

MonitorLobby::~MonitorLobby() {
    try {
        stop();
    } catch (...) {}
}

void MonitorLobby::init_dispatch() {
    action_dispatch = {
        {ClientAction::Type::Room, [this](ClientAction act){ handle_room_action(std::move(act)); }},
        {ClientAction::Type::Name, [this](ClientAction act){ handle_name_action(std::move(act)); }},
        {ClientAction::Type::Move, [this](ClientAction act){ handle_move_action(std::move(act)); }},
        {ClientAction::Type::StartGame, [this](ClientAction act){ handle_start_game(std::move(act)); }},
        {ClientAction::Type::ChooseCar, [this](ClientAction act){ handle_choose_car_action(std::move(act)); }},
        {ClientAction::Type::Improvement, [this](ClientAction act){ handle_improvement_action(std::move(act)); }}
    };
}

size_t MonitorLobby::reserve_connection_id() {
    std::lock_guard<std::mutex> lk(m);
    return next_conn_id++;
}

void MonitorLobby::add_pending_connection(std::shared_ptr<ClientHandler> ch, size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    pending.add_pending(conn_id, std::move(ch));
    // upon new pending, send current room list and start handler threads (same behavior que antes)
    pending.start_handler_threads_for(conn_id, rooms.list_rooms_with_counts(bindings));
}

std::vector<RoomInfo> MonitorLobby::list_rooms_locked() const {
    // RoomManager se encarga de filtrar salas iniciadas
    return rooms.list_rooms_with_counts(bindings);
}

void MonitorLobby::handle_room_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);

    if (act.room_cmd == ROOM_CREATE) {
        std::cout << "[Lobby] Processing ROOM_CREATE from conn_id=" << act.id << "\n";
        // detach from current room if present
        auto handler = rooms.detach_from_room(act.id, bindings, pending);
        if (handler) {
            pending.add_pending(act.id, handler);
            std::cout << "[Lobby] Moved conn_id=" << act.id << " from room to pending\n";
        }

        // create room
        uint8_t rid = rooms.create_room(/*max_players=*/8, act.id);
        // notify creator if still pending
        auto ph = pending.get_pending_handler(act.id);
        if (ph) {
            ph->send_room_created_to_client(rid);
        } else {
            std::cout << "[Lobby] ERROR: No pending handler for conn_id=" << act.id << "\n";
        }

    // broadcast update
    pending.broadcast_rooms(rooms.list_rooms_with_counts(bindings));

        // auto join creator
        if (pending.has_pending(act.id)) {
            if (rooms.join_room_from_pending(act.id, rid, pending, bindings)) {
                pending.broadcast_rooms(rooms.list_rooms_with_counts(bindings));
            } else {
                std::cout << "[Lobby] ERROR: Auto-join failed for conn_id=" << act.id << "\n";
            }
        }
    } else if (act.room_cmd == ROOM_JOIN) {
        std::cout << "[Lobby] JOIN request conn_id=" << act.id
                  << " -> room_id=" << (int)act.room_id << "\n";
        if (rooms.join_room_from_pending(act.id, act.room_id, pending, bindings)) {
            pending.broadcast_rooms(rooms.list_rooms_with_counts(bindings));
        }
    } else if (act.room_cmd == ROOM_LEAVE) {
        std::cout << "[Lobby] Processing ROOM_LEAVE from conn_id=" << act.id << "\n";
        auto handler = rooms.detach_from_room(act.id, bindings, pending);
        if (handler) {
            pending.add_pending(act.id, handler);
            std::cout << "[Lobby] Moved conn_id=" << act.id << " from room to pending\n";
            pending.broadcast_rooms(rooms.list_rooms_with_counts(bindings));
        } else {
            std::cout << "[Lobby] ROOM_LEAVE ignored: conn_id=" << act.id << " had no room\n";
            pending.broadcast_rooms(rooms.list_rooms_with_counts(bindings));
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
    auto binding = bindings.find_binding(act.id);
    if (!binding.has_value()) {
        pending.store_pending_name(act.id, std::move(act.username));
        std::cout << "[Lobby] Stored pending name for conn_id=" << act.id << "\n";
    } else {
        uint8_t rid = binding->first;
        size_t pid = binding->second;
        rooms.apply_player_name(rid, pid, std::move(act.username));
    }
}

void MonitorLobby::handle_move_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    auto binding = bindings.find_binding(act.id);
    if (!binding.has_value()) {
        // re-send rooms list to pending if present
        auto ph = pending.get_pending_handler(act.id);
        if (ph) {
            ph->send_rooms_to_client(rooms.list_rooms_with_counts(bindings));
            std::cout << "[Lobby] MOVE from conn_id=" << act.id
                      << " ignored (not in room). Sent rooms list again.\n";
        }
        return;
    }

    uint8_t rid = binding->first;
    size_t pid = binding->second;
    if (!rooms.push_move_to_room(rid, pid, act.movement)) {
        std::cout << "[Lobby] MOVE routing failed for conn_id=" << act.id << "\n";
    } else {
        std::cout << "[Lobby] Routed MOVE from conn_id=" << act.id
                  << " -> room_id=" << (int)rid
                  << ", player_id=" << pid << "\n";
    }
}

void MonitorLobby::handle_start_game(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    std::cout << "[MonitorLobby] conn_id=" << act.id << " solicitó START_GAME\n";

    auto binding = bindings.find_binding(act.id);
    if (!binding.has_value()) {
        std::cout << "[MonitorLobby] ERROR: conn_id=" << act.id << " no está en ninguna sala\n";
        return;
    }
    uint8_t target_room_id = binding->first;

    if (!rooms.is_creator(act.id, target_room_id)) {
        std::cout << "[MonitorLobby] ERROR: conn_id=" << act.id << " no es el creador/admin\n";
        return;
    }

    // mark started, load map, notify, etc.
    rooms.start_room_game(target_room_id, act, bindings, pending);
    // broadcast to pending about updated room state
    pending.broadcast_rooms(rooms.list_rooms_with_counts(bindings));
}

void MonitorLobby::handle_choose_car_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    pending.store_pending_car(act.id, act.car_id);
    std::cout << "[Lobby] Saved car_id=" << (int)act.car_id << " for conn_id=" << act.id << std::endl;
}

void MonitorLobby::handle_improvement_action(ClientAction act) {
    std::lock_guard<std::mutex> lk(m);
    // Debe estar dentro de una sala y con binding ya asignado a un player
    auto binding = bindings.find_binding(act.id);
    if (!binding.has_value()) {
        std::cout << "[Lobby] IMPROVEMENT ignored for conn_id=" << act.id << " (not in room)\n";
        return;
    }

    uint8_t rid = binding->first;
    size_t pid = binding->second;
    // Route hacia la Match interna para que el gameloop procese la compra (solo tendrá efecto si el Game está en Marketplace)
    if (!rooms.push_improvement_to_room(rid, pid, act.improvement_id)) {
        std::cout << "[Lobby] IMPROVEMENT routing failed for conn_id=" << act.id << " room_id=" << (int)rid << "\n";
    } else {
        std::cout << "[Lobby] Routed IMPROVEMENT from conn_id=" << act.id << " -> room_id=" << (int)rid
                  << ", player_id=" << pid << ", improvement_id=" << (int)act.improvement_id << "\n";
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
                    it->second(std::move(act));
                } else {
                    std::cout << "[Lobby] Acción desconocida ignorada\n";
                }

                std::cout << "[Lobby] ======================================\n";
            }

            {   // reap & housekeeping
                std::lock_guard<std::mutex> lk(m);
                // reap disconnected clients from room manager (which will update bindings/pending)
                rooms.reap(bindings, pending);
            }

            iteration_count++;
            if (iteration_count % 100 == 0) {
                std::lock_guard<std::mutex> lk(m);
                std::cout << "[Lobby] Status: " << rooms.room_count()
                          << " rooms, " << pending.count()
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

    { // shutdown: stop room loops and clear state
        std::lock_guard<std::mutex> lk(m);
        rooms.stop_all_rooms();
        pending.clear_all();
        bindings.clear_all();
    }
}

