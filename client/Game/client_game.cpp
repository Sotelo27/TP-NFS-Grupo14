#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#include <SDL2/SDL.h>

#include "sdl_wrappers/SdlDrawFill.h"

#include "constants.h"

ClientGame::ClientGame(size_t client_id, ServerHandler& server_handler, std::vector<PlayerResultTotal>& final_results):
        ConstantRateLoop(FRAME_RATE),
        client_id(client_id),
        server_handler(server_handler),
        final_results(final_results),
        info_players(),
        window(WINDOW_WIDTH, WINDOW_HEIGHT),
        map_manager(window),
        icon_improvement_manager(window),
        time_info(),
        cheat_detector(5),
        client_helper(client_id, window, info_players, map_manager, icon_improvement_manager,
                      time_info),
        intermission_manager(client_id, window, server_handler, map_manager, this->running,
                             icon_improvement_manager, client_helper, final_results) {}

void ClientGame::function() {
    update_state_from_position();

    // Clear display
    window.fill();

    client_helper.update_animation_frames();

    client_helper.render_in_z_order(iteration);
}

void ClientGame::start() {
    process_server_messages(ServerMessage::Type::RaceStart);

    std::cout << "[ClientGame] Juego iniciado" << std::endl;

    ConstantRateLoop::start_loop();
}

void ClientGame::handle_cheat_detection(const char* keyName) {
    cheat_detector.add_key(keyName);

    if (cheat_detector.check_cheat("Q")) {
        std::cout << "[ClientGame] Cheat code EXIT detected. Exiting game." << std::endl;
        running = false;
    }
    if (cheat_detector.check_cheat("MID")) {
        std::cout << "[ClientGame] Cheat code MID detected. Intermission!" << std::endl;
        // esto se borra después
        std::vector<PlayerResultCurrent>
                player_infos;  // en un futuro se recibe la lista de jugadores
        player_infos.push_back(PlayerResultCurrent{1, "Player1", 120, 300, 1});
        player_infos.push_back(PlayerResultCurrent{2, "Player2", 130, 320, 2});
        player_infos.push_back(PlayerResultCurrent{3, "Player3", 140, 350, 3});
        player_infos.push_back(PlayerResultCurrent{4, "Player4", 150, 400, 4});
        player_infos.push_back(PlayerResultCurrent{5, "Player5", 160, 450, 5});

        intermission_manager.run(player_infos, iteration);
    }
    // Cheat de vida infinita: tecla '7'
    if (std::string(keyName) == "7") {
        ClientMessage msg;
        msg.cheat = CHEAT_INFINITE_LIFE;
        server_handler.send_cheat(msg);
        std::cout << "[ClientGame] Cheat de vida infinita enviado al servidor.\n";
    }
    // Cheat de teletransporte: tecla '8'
    if (std::string(keyName) == "8") {
        ClientMessage msg;
        msg.cheat = CHEAT_TELEPORT_NEXT_CHECKPOINT;
        server_handler.send_cheat(msg);
        std::cout << "[ClientGame] Cheat de teletransporte enviado al servidor.\n";
    }
    // Cheat de ganar carrera automáticamente: tecla '9'
    if (std::string(keyName) == "9") {
        ClientMessage msg;
        msg.cheat = CHEAT_WIN_RACE;
        server_handler.send_cheat(msg);
        std::cout << "[ClientGame] Cheat WIN_RACE enviado al servidor.\n";
    }
}

void ClientGame::handle_sdl_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;

                const char* keyName = SDL_GetKeyName(keyEvent.keysym.sym);
                std::cout << "Tecla presionada: " << keyName << std::endl;

                handle_cheat_detection(keyName);
            } break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_QUIT:
                std::cout << "[ClientGame] Quit event received" << std::endl;
                running = false;

                break;
        }
    }
}

void ClientGame::handle_movement_input() {
    const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
    if (keyboard[SDL_SCANCODE_LEFT]) {
        server_handler.send_movement(Movement::Left);
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        server_handler.send_movement(Movement::Right);
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        server_handler.send_movement(Movement::Up);
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        server_handler.send_movement(Movement::Down);
    }
}

void ClientGame::process_server_messages(ServerMessage::Type expected_type, int msg_limit) {
    int msg_count = 0;
    bool keep_loop = true;

    while (keep_loop && (msg_limit == -1 || msg_count < msg_limit)) {
        ServerMessage action = server_handler.recv_response_from_server();

        if (action.type == ServerMessage::Type::MapInfo) {
            client_helper.update_map_info(action.players_tick, action.npcs_tick, action.race_time);
        } else if (action.type == ServerMessage::Type::RaceStart) {
            map_manager.loadMap(static_cast<MapID>(action.map_id));
        } else if (action.type == ServerMessage::Type::Results) {
            std::cout << "[ClientGame] Received RESULTS from server (n="
                      << action.results_current.size() << ")" << std::endl;

            intermission_manager.run(std::move(action.results_current), iteration);
        } else if (action.type == ServerMessage::Type::ResultsFinal) {
            std::cout << "[ClientGame] Received FINAL RESULTS from server (n="
                      << action.results_total.size() << ")" << std::endl;
            final_results = std::move(action.results_total);
            this->running = false;
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
            this->running = false;
            std::cout << "[ClientGame] Received Unknown message from server, probably "
                         "disconnected. Exiting..."
                      << std::endl;
        }

        if (action.type == expected_type) {
            keep_loop = false;
        }

        msg_count++;
    }
}


void ClientGame::update_state_from_position() {
    handle_sdl_events();

    handle_movement_input();

    process_server_messages(ServerMessage::Type::Empty, 10);
}

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

    info_players.clear();
}
