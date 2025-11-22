#include "binding_manager.h"
#include <mutex>

void BindingManager::add_binding(size_t conn_id, uint8_t room_id, size_t player_id) {
    std::lock_guard<std::mutex> lk(m);
    bindings[conn_id] = std::make_pair(room_id, player_id);
}

std::optional<std::pair<uint8_t, size_t>> BindingManager::remove_binding(size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = bindings.find(conn_id);
    if (it == bindings.end()) return std::nullopt;
    auto out = it->second;
    bindings.erase(it);
    return out;
}

std::optional<std::pair<uint8_t, size_t>> BindingManager::find_binding(size_t conn_id) const {
    std::lock_guard<std::mutex> lk(m);
    auto it = bindings.find(conn_id);
    if (it == bindings.end()) return std::nullopt;
    return it->second;
}

std::vector<size_t> BindingManager::conns_in_room(uint8_t room_id) const {
    std::vector<size_t> out;
    std::lock_guard<std::mutex> lk(m);
    for (const auto& kv : bindings) {
        if (kv.second.first == room_id) out.push_back(kv.first);
    }
    return out;
}

size_t BindingManager::count_in_room(uint8_t room_id) const {
    size_t c = 0;
    std::lock_guard<std::mutex> lk(m);
    for (const auto& kv : bindings) if (kv.second.first == room_id) ++c;
    return c;
}

void BindingManager::clear_all() {
    std::lock_guard<std::mutex> lk(m);
    bindings.clear();
}
