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

void Client::open_game_window(size_t& my_id, ServerHandler& server_handler, std::vector<PlayerResultTotal>& final_results, MapID selected_map) {
    ClientGame game(my_id, server_handler, final_results);
    game.start(selected_map);
}

void Client::start() {
    std::cout << "[Client] Starting server handler threads..." << std::endl;
    server_handler.start();
    
    int n = 0;
    QApplication app(n, nullptr);

    size_t my_id = SIZE_MAX;
    MapID selected_map;
    bool map_selected = false;
    game_window_start = new GameWindow(server_handler, my_id, map_selected, selected_map, true, nullptr);
    game_window_start->exec();
    close_QT_window(game_window_start);

    if (my_id == SIZE_MAX ) {
        std::cout << "[Client] Exiting..." << std::endl;
        return;
    }

    if (!map_selected) {
        std::cout << "[Client] No elegiste mapa..." << std::endl;
        return;
    }


    std::cout << "[Client] Starting game loop..." << std::endl;
    std::cout << "[Client] My client ID is: " << my_id << std::endl;

    std::vector<PlayerResultTotal> final_results;
    open_game_window(my_id, server_handler, final_results, selected_map);

    if (final_results.empty()) {
        std::cout << "[Client] Game ended unexpectedly. Exiting..." << std::endl;
        return;
    }

    for (const auto& result : final_results) {
        std::cout << "[Client] Player position " << static_cast<int>(result.position)
                  << ": " << result.username
                  << " with total time " << result.total_time_seconds << "s" << std::endl;
    }

    game_window_end = new GameWindow(server_handler, my_id, map_selected, selected_map, false, nullptr);
    game_window_end->setFinalResults(final_results);
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
