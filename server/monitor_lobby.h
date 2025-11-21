#ifndef MONITOR_LOBBY_H
#define MONITOR_LOBBY_H

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "../common/queue.h"
#include "../common/thread.h"
#include "client_handler.h"
#include "client_list.h"
#include "game/gameloop.h"
#include "Match.h"

#include "room_manager.h"
#include "pending_manager.h"
#include "binding_manager.h"

class MonitorLobby : public Thread {
public:
    explicit MonitorLobby(float nitro_duracion);
    ~MonitorLobby() override;

    MonitorLobby(const MonitorLobby&) = delete;
    MonitorLobby& operator=(const MonitorLobby&) = delete;
    MonitorLobby(MonitorLobby&&) = delete;
    MonitorLobby& operator=(MonitorLobby&&) = delete;

    // Cola global para que Acceptor/ClientHandler encolen acciones
    Queue<ClientAction>& incoming_actions() { return actions_in; }

    // Reservar id para nueva conexión
    size_t reserve_connection_id();

    // Registrar conexión pendiente (toma ownership moviendo el objeto)
    void add_pending_connection(std::shared_ptr<ClientHandler> ch, size_t conn_id);

    void run() override;

    void stop() override {
        Thread::stop();
        actions_in.close();
    }

private:
    // global incoming actions
    Queue<ClientAction> actions_in;

    // Subcomponentes que encapsulan la lógica anterior
    RoomManager rooms;
    PendingManager pending;
    BindingManager bindings;

    // Generadores de ids
    size_t next_conn_id{1};

    float nitro_duracion;
    std::mutex m;

    // dispatch
    std::unordered_map<ClientAction::Type, std::function<void(ClientAction)>> action_dispatch;
    void init_dispatch();

    // handlers (delegan en los managers)
    void handle_room_action(ClientAction act);
    void handle_name_action(ClientAction act);
    void handle_move_action(ClientAction act);
    void handle_start_game(ClientAction act);
    void handle_choose_car_action(ClientAction act);

    // helpers
    std::vector<RoomInfo> list_rooms_locked() const;
};

#endif // MONITOR_LOBBY_H
