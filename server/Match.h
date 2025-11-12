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
    Game game;
    ClientListProtected clients;
    Queue<ClientAction> actions;
    std::optional<Gameloop> loop;
    uint8_t max_players{8};

    explicit Match(uint8_t id, float nitro_duracion, uint8_t max_players):
        room_id(id),
        game(nitro_duracion),
        clients(),
        actions(),
        loop(std::nullopt),
        max_players(max_players) {}
};

#endif
