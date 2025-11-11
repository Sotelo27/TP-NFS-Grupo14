#include "gameloop.h"

#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../common/constants.h"

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

void Gameloop::iteracion_game() {
    const float dt = 0.016f;
    game.update(dt);
    auto tick_players = game.players_tick_info();
    // Por ahora no hay NPC, eventos dinamicos lo de las colisiones pero aun no lleno esa info
    std::vector<NpcTickInfo> npcs;
    std::vector<EventInfo> events;
    clients.broadcast_map_info(tick_players, npcs, events);
}

void Gameloop::run() {
    while (should_keep_running()) {
        try {
            procesar_actiones();
            iteracion_game();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        } catch (const std::exception& err) {
            std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        }
    }
}

Gameloop::~Gameloop() { actiones_clients.close(); }
