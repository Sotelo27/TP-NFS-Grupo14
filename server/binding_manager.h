#ifndef BINDING_MANAGER_H
#define BINDING_MANAGER_H

#include <cstdint>
#include <optional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <optional>   


class BindingManager {
public:
    BindingManager() = default;

    // add binding conn_id -> (room_id, player_id)
    void add_binding(size_t conn_id, uint8_t room_id, size_t player_id);

    // remove binding for conn_id, returns optional pair (room_id, player_id)
    std::optional<std::pair<uint8_t, size_t>> remove_binding(size_t conn_id);

    // find binding for conn_id
    std::optional<std::pair<uint8_t, size_t>> find_binding(size_t conn_id) const;

    // get connection ids in room
    std::vector<size_t> conns_in_room(uint8_t room_id) const;

    // count conns in room
    size_t count_in_room(uint8_t room_id) const;

    // clear all bindings
    void clear_all();

private:
    mutable std::mutex m;
    std::unordered_map<size_t, std::pair<uint8_t, size_t>> bindings;
};
#endif // BINDING_MANAGER_H
