#ifndef SERVER_MATCH_H
#define SERVER_MATCH_H

#include <cstdint>
#include <optional>

#include "../common/queue.h"
#include "client_action.h"
#include "game/game.h"
#include "client_list.h"
#include "game/gameloop.h"

struct Match {
    uint8_t room_id{0};
    uint8_t max_players{8};
    size_t creator_conn_id;  // ID del creador de la sala (admin permanente)
    Game game;
    ClientListProtected clients;
    Queue<ClientAction> actions;
    std::optional<Gameloop> loop;
    bool started{false}; // <-- NUEVO: indica si la sala ya inició

    explicit Match(uint8_t id, float nitro_duracion, uint8_t max_players, size_t creator_id):
        room_id(id),
        max_players(max_players),
        creator_conn_id(creator_id),
        game(nitro_duracion),
        clients(),
        actions(),
        loop(std::nullopt),
        started(false) // <-- inicializar en false
    {}
};

#endif
