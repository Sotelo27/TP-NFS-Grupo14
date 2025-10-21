#include "gameloop.h"

#include <chrono>
#include <iostream>
#include <list>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "../common/constants.h"

Gameloop::Gameloop(Game& game, ClientListProtected& clients,
                   Queue<ClientAction>& actiones_clients):
        game(game), clients(clients), actiones_clients(actiones_clients) {}

void Gameloop::procesar_actiones() {
    ClientAction action;
    while (actiones_clients.try_pop(action)) {
        try {
            if (action.action == CODE_CLIENT_NITRO) {
                if (game.jugador_activar_nitro(action.id)) {
                    clients.broadcast_nitro_activado(game.cantidad_nitros_activos());
                    std::cout << HIT_NITRO << std::endl;
                }
            } else {
                throw std::runtime_error("Unknown action code received from client.");
            }
        } catch (const std::exception& err) {
            std::cerr << "Error processing action from client " << action.id << ": " << err.what()
                      << "\n";
        }
    }
}

void Gameloop::iteracion_game() {
    size_t cantidad_nitros = game.cantidad_nitros_activos();
    std::vector<size_t> ids = game.tiempo_trascurrido(0.25f);
    for (size_t i = ids.size(); i > 0; i--) {
        cantidad_nitros--;
        clients.broadcast_nitro_desactivado(cantidad_nitros);
        std::cout << OUT_NITRO << std::endl;
    }
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
