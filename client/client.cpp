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

void Client::open_game_window(size_t& my_id, ServerHandler& server_handler, bool& game_is_over) {
    ClientGame game(my_id, server_handler, game_is_over);
    game.start();
}

void Client::start() {
    std::cout << "[Client] Starting server handler threads..." << std::endl;
    server_handler.start();
    
    int n = 0;
    QApplication app(n, nullptr);

    size_t my_id = SIZE_MAX;
    bool map_selected = false;
    game_window_start = new GameWindow(server_handler, my_id, map_selected, true, nullptr);
    game_window_start->exec();
    close_QT_window(game_window_start);

    if (my_id == SIZE_MAX || !map_selected) {
        std::cout << "[Client] Exiting..." << std::endl;
        return;
    }

    std::cout << "[Client] Starting game loop..." << std::endl;
    std::cout << "[Client] My client ID is: " << my_id << std::endl;

    bool game_is_over = false;
    open_game_window(my_id, server_handler, game_is_over);

    if (!game_is_over) {
        std::cout << "[Client] Game ended unexpectedly. Exiting..." << std::endl;
        return;
    }

    game_window_end = new GameWindow(server_handler, my_id, map_selected, false, nullptr);
    game_window_end->exec();
    close_QT_window(game_window_end);

}

void Client::close_QT_window(GameWindow* &game_window) {
    if (game_window) {
        delete game_window;
        game_window = nullptr;
    }
}

Client::~Client() {
    if (game_window_start) {
        delete game_window_start;
        game_window_start = nullptr;
    }
    if (game_window_end) {
        delete game_window_end;
        game_window_end = nullptr;
    }
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

}
