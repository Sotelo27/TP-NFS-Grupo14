#include "gameloop.h"

#include <chrono>
#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../../common/constants.h"

#define SERVER_HZ 60
#define BROADCAST_HZ 30

Gameloop::Gameloop(Game& game, ClientListProtected& clients, Queue<ClientAction>& actiones_clients):
    game(game), clients(clients), actiones_clients(actiones_clients), loop(SERVER_HZ) {
        ticks_per_broadcast = std::max(1, SERVER_HZ / BROADCAST_HZ);
    }

void Gameloop::procesar_actiones() {
    ClientAction action;
    while (actiones_clients.try_pop(action)) {
        try {
            if (action.type == ClientAction::Type::Move) {
                game.register_player_move(action.id, action.movement);
            } else if (action.type == ClientAction::Type::Name) {
                game.set_player_name(action.id, std::move(action.username));
            } else if (action.type == ClientAction::Type::Room) {
                std::cout << "Room action from client " << action.id
                          << " cmd=" << (int)action.room_cmd << " room=" << (int)action.room_id
                          << "\n";
            }

        } catch (const std::exception& err) {
            std::cerr << "Error processing action from client " << action.id << ": " << err.what()
                      << "\n";
        }
    }
}

void Gameloop::func_tick(int iteration) {
    procesar_actiones();
    game.update(1.0f / SERVER_HZ);

    if (iteration % ticks_per_broadcast == 0) {
        auto tick_players = game.players_tick_info();
        TimeTickInfo time_race = game.get_race_time();
        std::vector<NpcTickInfo> npcs;
        std::vector<EventInfo> events;

        clients.broadcast_map_info(tick_players, npcs, events, time_race);
    }
}

void Gameloop::run() {
    loop.start_loop([this](int iteration) { func_tick(iteration); },
                    [this]() { return should_keep_running(); });
}

Gameloop::~Gameloop() { actiones_clients.close(); }
