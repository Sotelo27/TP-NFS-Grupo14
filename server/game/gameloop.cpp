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
#include "../../common/enum/car_improvement.h"

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

            }else if (action.type == ClientAction::Type::Improvement) {
                std::cout << "[Gameloop] Processing IMPROVEMENT for player_id="
                          << action.id << " imp=" << (int)action.improvement_id << "\n";
                
                CarImprovement imp = (CarImprovement)(action.improvement_id);
                bool ok = game.buy_upgrade(action.id, imp);
                PlayerMarketInfo info = game.get_player_market_info(action.id);
                ImprovementResult result{};
                result.player_id = (uint32_t)action.id;
                result.improvement_id = (uint8_t)action.improvement_id;
                result.ok = ok;
                float cost = ok ? game.get_improvement_penalty(imp) : 0.f;
                result.total_penalty_seconds = (uint32_t)std::round(cost);
                result.current_balance = (uint32_t)std::round(info.balance);
                clients.broadcast_improvement_ok(result);
            }
            else if (action.type == ClientAction::Type::Cheat) {
                std::cout << "[Gameloop] Processing CHEAT for player_id=" << action.id
                          << " cheat=" << (int)action.cheat << "\n";
                game.apply_cheat(action.id, action.cheat);
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

    uint8_t next_map_id;
    if (game.consume_pending_race_start(next_map_id)) {
            std::cout << "[Gameloop] Consumed pending RaceStart, map_id=" << (int)next_map_id << "\n";
            clients.broadcast_race_start(next_map_id);
        std::cout << "[Gameloop] RaceStart broadcast completed (map_id=" << (int)next_map_id << ")\n";
    }

    if (game.has_pending_results()) {
        std::vector<PlayerResultCurrent> curr;
        if (game.comsume_pending_results(curr)) {
            clients.broadcast_results(curr);
        }
    }

    std::vector<ImprovementResult> init_msgs;
    if (game.consume_pending_market_init(init_msgs)) {
        std::cout << "[Gameloop] Broadcasting MARKET INIT (after results) n=" << init_msgs.size() << "\n";
        for (const auto& msg : init_msgs) {
            clients.broadcast_improvement_ok(msg);
        }
    }

    if (game.has_pending_total_results()) {
        std::vector<PlayerResultTotal> total;
        if (game.consume_pending_total_results(total)) {
            clients.broadcast_results_total(total);
        }
    }

    if (iteration % ticks_per_broadcast == 0) {
        std::vector<NpcTickInfo> npcs;
        std::vector<EventInfo> events;

        if (game.has_active_race()) {
            auto tick_players = game.players_tick_info();
            auto tick_npcs = game.npcs_tick_info();
            TimeTickInfo time_race = game.get_race_time();
            clients.broadcast_map_info(tick_players, tick_npcs, events, time_race);
        } else if (game.has_active_market_place()) {
            TimeTickInfo time_market = game.get_market_time();
            clients.broadcast_market_time_info(time_market);
        }
    }
}

void Gameloop::run() {
    loop.start_loop([this](int iteration) { func_tick(iteration); },
                    [this]() { return should_keep_running(); });
}

Gameloop::~Gameloop() { actiones_clients.close(); }
