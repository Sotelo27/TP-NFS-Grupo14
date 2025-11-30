#ifndef ROOM_MANAGER_H
#define ROOM_MANAGER_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "Match.h"
#include "client_handler.h"
#include "client_list.h"
#include "../common/constants.h"
#include "binding_manager.h"
#include "pending_manager.h"

class RoomManager {
public:
    explicit RoomManager(float nitro_duracion);

    // create room and start its loop
    uint8_t create_room(uint8_t max_players, size_t creator_conn_id);

    // list rooms (skips started) without counts (legacy)
    std::vector<RoomInfo> list_rooms() const;

    // list rooms including current player counts using bindings
    std::vector<RoomInfo> list_rooms_with_counts(const BindingManager& bindings) const;

    // join a room from pending manager - moves handler from pending into room; returns success
    bool join_room_from_pending(size_t conn_id, uint8_t room_id, PendingManager& pending, BindingManager& bindings);

    // detach conn_id from current room and return handler to re-add as pending
    std::shared_ptr<ClientHandler> detach_from_room(size_t conn_id, BindingManager& bindings, PendingManager& pending);

    // push a move action into room's Match.actions (returns success)
    bool push_move_to_room(uint8_t room_id, size_t player_id, Movement movement);
    // push an improvement action into room's Match.actions (returns success)
    bool push_improvement_to_room(uint8_t room_id, size_t player_id, uint8_t improvement_id);

    // reap disconnected clients in rooms (calls clients.reap and remove players from games)
    void reap(BindingManager& bindings, PendingManager& pending);

    // stop all rooms and room loops
    void stop_all_rooms();

    // number of rooms
    size_t room_count() const;

    // check if a conn is creator of a room
    bool is_creator(size_t conn_id, uint8_t room_id) const;

    // start the room's game (load map, start race, send RaceStart)
    void start_room_game(uint8_t room_id, const ClientAction& startAction, BindingManager& bindings, PendingManager& pending);

    // apply a player name to a player inside a room
    void apply_player_name(uint8_t room_id, size_t player_id, std::string name);

    // stop specific room loop and clear rooms
    void stop_room_loop_and_erase(uint8_t room_id);

    // NUEVO: setear vida infinita a un jugador
    void set_player_infinite_life(uint8_t room_id, size_t player_id, bool enable);

private:
    mutable std::mutex m;
    std::map<uint8_t, Match> rooms;
    uint8_t next_room_id{1};
    float nitro_duracion;

    // helpers (need lock)
    void start_room_loop_locked(Match& p);
    void stop_room_loop_locked(Match& p);
};
#endif // ROOM_MANAGER_H
