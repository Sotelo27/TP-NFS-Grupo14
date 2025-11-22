#include "pending_manager.h"
#include <iostream>

void PendingManager::add_pending(size_t conn_id, std::shared_ptr<ClientHandler> handler) {
    std::lock_guard<std::mutex> lk(m);
    pending_handlers[conn_id] = std::move(handler);
    std::cout << "[PendingManager] New pending connection conn_id=" << conn_id << "\n";
}

std::shared_ptr<ClientHandler> PendingManager::take_pending(size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = pending_handlers.find(conn_id);
    if (it == pending_handlers.end()) return nullptr;
    auto h = it->second;
    pending_handlers.erase(it);
    return h;
}

std::shared_ptr<ClientHandler> PendingManager::get_pending_handler(size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = pending_handlers.find(conn_id);
    if (it == pending_handlers.end()) return nullptr;
    return it->second;
}

bool PendingManager::has_pending(size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    return pending_handlers.find(conn_id) != pending_handlers.end();
}

void PendingManager::start_handler_threads_for(size_t conn_id, const std::vector<RoomInfo>& rooms_list) {
    std::shared_ptr<ClientHandler> h = get_pending_handler(conn_id);
    if (!h) return;
    h->send_rooms_to_client(rooms_list);
    std::cout << "[PendingManager] Starting recv and send threads for conn_id=" << conn_id << "\n";
    h->start_recv_only();
    h->start_send_only();
}

void PendingManager::broadcast_rooms(const std::vector<RoomInfo>& rooms_list) {
    std::lock_guard<std::mutex> lk(m);
    for (auto& kv : pending_handlers) {
        if (kv.second) kv.second->send_rooms_to_client(rooms_list);
    }
}

void PendingManager::store_pending_name(size_t conn_id, std::string name) {
    std::lock_guard<std::mutex> lk(m);
    pending_names[conn_id] = std::move(name);
}

std::optional<std::string> PendingManager::take_pending_name(size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = pending_names.find(conn_id);
    if (it == pending_names.end()) return std::nullopt;
    std::string out = std::move(it->second);
    pending_names.erase(it);
    return out;
}

void PendingManager::store_pending_car(size_t conn_id, uint8_t car_id) {
    std::lock_guard<std::mutex> lk(m);
    pending_car_id[conn_id] = car_id;
}

std::optional<uint8_t> PendingManager::take_pending_car(size_t conn_id) {
    std::lock_guard<std::mutex> lk(m);
    auto it = pending_car_id.find(conn_id);
    if (it == pending_car_id.end()) return std::nullopt;
    uint8_t out = it->second;
    pending_car_id.erase(it);
    return out;
}

size_t PendingManager::count() const {
    std::lock_guard<std::mutex> lk(m);
    return pending_handlers.size();
}

void PendingManager::clear_all() {
    std::lock_guard<std::mutex> lk(m);
    pending_handlers.clear();
    pending_names.clear();
    pending_car_id.clear();
}
