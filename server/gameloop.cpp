#include "gameloop.h"

#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../common/constants.h"

#define SERVER_HZ 60
#define BROADCAST_HZ 20

Gameloop::Gameloop(Game& game, ClientListProtected& clients, Queue<ClientAction>& actiones_clients):
    game(game), clients(clients), actiones_clients(actiones_clients) {}

void Gameloop::procesar_actiones() {
    ClientAction action;
    while (actiones_clients.try_pop(action)) {
        try {
            std::cout << "Received action from client " << action.id << ": "
                      << "Type=" << (int)(action.type)
                      << ", Username=" << action.username
                      << ", Movement=" << (int)(action.movement) << "\n";

            if (action.type == ClientAction::Type::Move) {
                // Aplicar movimiento en el dominio
                game.apply_player_move(action.id, action.movement);
            } else if (action.type == ClientAction::Type::Name) {
                std::cout << "Bienvenido " << action.username << " (id " << action.id << ")\n";
                game.set_player_name(action.id, std::move(action.username));
                // Aqui faltaria lo de enviar OK al cliente por su hilo de envio
            } else if (action.type == ClientAction::Type::Room) {
                std::cout << "Room action from client " << action.id
                          << " cmd=" << (int)action.room_cmd << " room=" << (int)action.room_id
                          << "\n";
                // TODO: Integrar con MonitorLobby (crear/unirse y validar cupo) - ahora lo maneja
                // MonitorLobby
            }

        } catch (const std::exception& err) {
            std::cerr << "Error processing action from client " << action.id << ": " << err.what()
                      << "\n";
        }
    }
}

//void Gameloop::iteracion_game() {
//    const float dt = 0.016f;
//    game.update(dt);
//    auto tick_players = game.players_tick_info();
//    // Por ahora no hay NPC, eventos dinamicos lo de las colisiones pero aun no lleno esa info
//    std::vector<NpcTickInfo> npcs;
//    std::vector<EventInfo> events;
//    clients.broadcast_map_info(tick_players, npcs, events);
//}

void Gameloop::run() {
    const int period_ms = std::max(1, 1000 / SERVER_HZ);// esto es casi equivalente a lo que sseri 16ms o 60fps , rate
    const int ticks_per_broadcast = std::max(1, (SERVER_HZ + BROADCAST_HZ - 1) / BROADCAST_HZ);
    const int max_catchup_steps = 4;

    auto prev = std::chrono::steady_clock::now(); // marca ideal del proximo tick, lo que seria el t1 :P
    int tick_count = 0;

    while (should_keep_running()) {
        try {
            procesar_actiones();

            const int before = tick_count;
            game.update(1.0f / SERVER_HZ);
            ++tick_count;

            // Se mide cuanto tardo el tick real
            auto now   = std::chrono::steady_clock::now(); //Este  es el tiempo 2 -> t2
            auto spent = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev); // Tiempo transcurrido desde el tick anterior, t2 - t1
            // calculamos la diferencia entre el tiempo ideal y el real
            // osea equivale a rest -> 16ms - (t2 - t1)
            auto delta = std::chrono::milliseconds(period_ms) - spent; 
            //Entonces:
            // si-> delta > 0 : Vamos a tiempo
            // si-> delta < 0 : vamos atradsados y tenemos que "ponernos al dia"

            if (delta > std::chrono::milliseconds(0)) {
                // Caso “vamos a tiempo”
                // Dormimos hasta el proximo tick exacto
                std::this_thread::sleep_for(delta);
                prev += std::chrono::milliseconds(period_ms); // Avanzamos la marca ideal (next_tick += frame)
            } else {
                // Caso “vamos atrasados”
                // (-delta) = tiempo que estamos atrasados -> es equivalente a a lo que vi en behind
                auto late_ms = -delta.count();
                int frames_late = (int)((late_ms + period_ms - 1) / period_ms) + 1; // cuantos frames deberiamos pagar

                // Limitamos el catch-up a unos pocos steps
                int catchup = std::min(frames_late, max_catchup_steps);

                for (int i = 0; i < catchup; ++i) {
                    procesar_actiones();
                    game.update(1.0f / SERVER_HZ);
                    ++tick_count;
                }

                //Aquie aplicamos el Rest: osea saltamos los frames que no pudimos pagar, para sincronizarnos
                prev += std::chrono::milliseconds(period_ms) * (1 + frames_late);
            }

            //Un solo broadcast si cruzamos el umbral de N ticks
            const int after = tick_count;
            const bool crossed =
                (before / ticks_per_broadcast) < (after / ticks_per_broadcast);
            if (crossed) {
                auto tick_players = game.players_tick_info();
                std::vector<NpcTickInfo> npcs;
                std::vector<EventInfo> events;
                clients.broadcast_map_info(tick_players, npcs, events);
            }

        } catch (const std::exception& err) {
            std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        }
    }
}

Gameloop::~Gameloop() { actiones_clients.close(); }
