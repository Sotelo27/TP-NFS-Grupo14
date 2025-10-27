#include "gameloop.h"

#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../common/constants.h"
#include "../common/base_protocol.h"

Gameloop::Gameloop(Game& game, ClientListProtected& clients,
                   Queue<ClientAction>& actiones_clients):
        game(game), clients(clients), actiones_clients(actiones_clients) {}

void Gameloop::procesar_actiones() {
    ClientAction action;
    while (actiones_clients.try_pop(action)) {
        try {
            // Aplicar movimiento en el dominio
            game.apply_player_move(action.id,(action.action));

        } catch (const std::exception& err) {
            std::cerr << "Error processing action from client " << action.id << ": " << err.what()
                      << "\n";
        }
    }
}

void Gameloop::iteracion_game() {
    const float dt = 0.25f; // 250 ms
    game.update(dt);
    auto positions = game.players_positions();
    clients.broadcast_player_positions(positions);
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
