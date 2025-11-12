#ifndef MONITOR_LOBBY_H
#define MONITOR_LOBBY_H

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <functional>          
#include <mutex>

#include "../common/constants.h"
#include "../common/player_aux.h"
#include "../common/queue.h"
#include "../common/thread.h"

#include "client_handler.h"
#include "client_list.h"
#include "game.h"
#include "gameloop.h"
#include "Match.h"

class MonitorLobby: public Thread {
private:
    // Acciones entrantes (global, previo a conocer sala)
    Queue<ClientAction> actions_in;

    // Conexiones en estado "pendiente"
    std::map<size_t, std::shared_ptr<ClientHandler>> pending;

    // Salas activas
    std::map<uint8_t, Match> rooms;

    // Vinculación connId -> (room_id, player_id en Game)
    std::unordered_map<size_t, std::pair<uint8_t, size_t>> bindings;

    // Nombres pendientes
    std::unordered_map<size_t, std::string> pending_names;

    // Generadores de ids
    size_t next_conn_id{1};
    uint8_t next_room_id{1};

    float nitro_duracion;
    std::mutex m;

    // Helpers (requieren m tomada)
    std::vector<RoomInfo> list_rooms_locked() const;
    void broadcast_rooms_to_pending_locked();
    void broadcast_players_in_room_locked(uint8_t room_id);
    std::vector<PlayerInfo> get_players_in_room_locked(uint8_t room_id) const;
    uint8_t create_room_locked(uint8_t max_players);
    bool join_room_locked(size_t conn_id, uint8_t room_id);
    void start_room_loop_locked(Match& p);
    void stop_room_loop_locked(Match& p);
    void reap_locked();

    //dispatch de acciones
    std::unordered_map<ClientAction::Type, std::function<void(ClientAction)>> action_dispatch;
    void init_dispatch();
    void handle_room_action(ClientAction act);  // maneja ROOM_CREATE / ROOM_JOIN
    void handle_name_action(ClientAction act);
    void handle_move_action(ClientAction act);
    void handle_start_game(ClientAction act); 

public:
    explicit MonitorLobby(float nitro_duracion);
    ~MonitorLobby() override;  // Apagado ordenado 

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
};

#endif
