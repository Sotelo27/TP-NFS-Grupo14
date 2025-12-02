#ifndef PENDING_MANAGER_H
#define PENDING_MANAGER_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "client_handler.h"
#include "client_list.h"
#include "Match.h"
#include "../common/constants.h"

class PendingManager {
public:
    PendingManager() = default;

    // add pending handler (moves ownership)
    void add_pending(size_t conn_id, std::shared_ptr<ClientHandler> handler);

    // remove and return pending handler if exists
    std::shared_ptr<ClientHandler> take_pending(size_t conn_id);

    // get (non-owning) pending handler
    std::shared_ptr<ClientHandler> get_pending_handler(size_t conn_id);

    // whether conn_id is pending
    bool has_pending(size_t conn_id);

    // send current rooms list to specific pending conn
    void start_handler_threads_for(size_t conn_id, const std::vector<RoomInfo>& rooms_list);

    // broadcast rooms list to all pending
    void broadcast_rooms(const std::vector<RoomInfo>& rooms_list);

    // store pending name / car id for a future join
    void store_pending_name(size_t conn_id, std::string name);
    std::optional<std::string> take_pending_name(size_t conn_id);

    void store_pending_car(size_t conn_id, uint8_t car_id);
    std::optional<uint8_t> take_pending_car(size_t conn_id);

    // count of pending
    size_t count() const;

    // clear all pending (and stop send/recv threads if needed)
    void clear_all();

private:
    mutable std::mutex m;
    std::unordered_map<size_t, std::shared_ptr<ClientHandler>> pending_handlers;
    std::unordered_map<size_t, std::string> pending_names;
    std::unordered_map<size_t, uint8_t> pending_car_id;
};
#endif // PENDING_MANAGER_H
