#ifndef MONITOR_LOBBY_H
#define MONITOR_LOBBY_H

#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../common/queue.h"
#include "../common/thread.h"
#include "../common/base_protocol.h"
#include "../common/constants.h"
#include "../common/player_aux.h"

#include "client_handler.h"
#include "client_list.h"
#include "gameloop.h"
#include "game.h"

class MonitorLobby : public Thread {
private:
    struct Partida {
        uint8_t room_id{0};
        Game game;
        ClientListProtected clients;
        Queue<ClientAction> actions;
        std::unique_ptr<Gameloop> loop;
        uint8_t max_players{8};

        explicit Partida(uint8_t id, float nitro_duracion, uint8_t max_players)
            : room_id(id), game(nitro_duracion), actions(), loop(nullptr), max_players(max_players) {}
    };

    // Acciones entrantes (global, previo a conocer sala)
    Queue<ClientAction> actions_in;

    // Conexiones en estado "pendiente" (aún no eligieron sala)
    std::map<size_t, std::unique_ptr<ClientHandler>> pending;

    // Salas activas
    std::map<uint8_t, Partida> rooms;

    // Vinculación connId -> (room_id, player_id en Game)
    std::unordered_map<size_t, std::pair<uint8_t, size_t>> bindings;

    // Nombres pendientes (antes de entrar a sala)
    std::unordered_map<size_t, std::string> pending_names;

    // Generadores de ids
    size_t next_conn_id{1};
    uint8_t next_room_id{1};

    float nitro_duracion;
    std::mutex m;

    // Helpers (requieren m tomada)
    std::vector<RoomInfo> list_rooms_locked() const;
    void broadcast_rooms_to_pending_locked();
    uint8_t create_room_locked(uint8_t max_players);
    bool join_room_locked(size_t conn_id, uint8_t room_id);
    void reap_locked();

public:
    explicit MonitorLobby(float nitro_duracion);

    // Cola global para que Acceptor/ClientHandler encolen acciones
    Queue<ClientAction>& incoming_actions() { return actions_in; }

    // Reservar id para nueva conexión
    size_t reserve_connection_id();

    // Registrar conexión pendiente (toma ownership del handler)
    void add_pending_connection(std::unique_ptr<ClientHandler> ch, size_t conn_id);

    void run() override;

    void stop() override { Thread::stop(); actions_in.close(); }
};

#endif
