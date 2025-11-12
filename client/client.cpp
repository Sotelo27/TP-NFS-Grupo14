#include "client.h"

#include "Game/client_game.h"
#include <iostream>
#include "../common/socket.h"
#include "QT/game_window.h"
#include <QApplication>

Client::Client(const char* hostname, const char* servname):
        server_handler(Socket(hostname, servname)), game_window_start(nullptr), game_window_end(nullptr) {
    std::cout << "[Client] Connected to server " << hostname << ":" << servname << std::endl;
}

void Client::start() {
    std::cout << "[Client] Starting server handler threads..." << std::endl;
    server_handler.start();
    
    int n = 0;
    QApplication app(n, nullptr);

    size_t my_id = SIZE_MAX;
    game_window_start = new GameWindow(server_handler, my_id, true);
    game_window_start->exec();

    if (my_id == SIZE_MAX) {
        std::cout << "[Client] Exiting..." << std::endl;
        return;
    }

    std::cout << "[Client] Starting game loop..." << std::endl;
    std::cout << "[Client] My client ID is: " << my_id << std::endl;
    bool game_is_over = false;
    ClientGame game(my_id, server_handler, game_is_over);
    game.start();

    if (!game_is_over) {
        std::cout << "[Client] Game ended unexpectedly. Exiting..." << std::endl;
        return;
    }

    game_window_end = new GameWindow(server_handler, my_id, false);
    game_window_end->exec();
}

Client::~Client() {
    if (game_window_start) {
        free(game_window_start);
    }
    if (game_window_end) {
        free(game_window_end);
    }
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

}
