#include "gameloop.h"

#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../common/base_protocol.h"
#include "../common/constants.h"

Gameloop::Gameloop(Game& game, ClientListProtected& clients, Queue<ClientAction>& actiones_clients):
        game(game), clients(clients), actiones_clients(actiones_clients) {}

void Gameloop::procesar_actiones() {
    ClientAction action;
    while (actiones_clients.try_pop(action)) {
        try {
            std::cout << "Received action from client " << action.id << ": "
                      << "Type=" << static_cast<int>(action.type)
                      << ", Username=" << action.username
                      << ", Movement=" << static_cast<int>(action.movement) << "\n";

            if (action.type == ClientAction::Type::Move) {
                // Aplicar movimiento en el dominio
                game.apply_player_move(action.id, action.movement);
            } else if (action.type == ClientAction::Type::Name) {
                std::cout << "Bienvenido " << action.username << " (id " << action.id << ")\n";
                game.set_player_name(action.id, std::move(action.username));
                // Aqui faltaria lo de enviar OK al cliente por su hilo de envio
            } else if (action.type == ClientAction::Type::Room) {
                std::cout << "Room action from client " << action.id
                          << " cmd=" << (int)action.room_cmd
                          << " room=" << (int)action.room_id << "\n";
                // TODO: Integrar con MonitorLobby (crear/unirse y validar cupo) - ahora lo maneja MonitorLobby
            }

        } catch (const std::exception& err) {
            std::cerr << "Error processing action from client " << action.id << ": " << err.what()
                      << "\n";
        }
    }
}

void Gameloop::iteracion_game() {
    const float dt = 0.25f;  // 250 ms
    game.update(dt);
    auto positions = game.players_positions();
    clients.broadcast_player_positions(positions);
    std::cout << "Broadcasted positions of " << positions.size() << " players.\n";
}

void Gameloop::run() {
    while (should_keep_running()) {
        try {
            procesar_actiones();
            iteracion_game();
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        } catch (const std::exception& err) {
            std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        }
    }
}

Gameloop::~Gameloop() { actiones_clients.close(); }
