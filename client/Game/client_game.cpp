#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "sdl_wrappers/SdlDrawFill.h"

#include "constants.h"

#define FRAME_RATE 60.0

ClientGame::ClientGame(size_t client_id, ServerHandler& server_handler, bool& game_is_over):
        ConstantRateLoop(FRAME_RATE),
        client_id(client_id),
        server_handler(server_handler),
        game_is_over(game_is_over),
        src_area_map(0, 0, 0, 0),
        dest_area_map(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
        info_players(),
        window(WINDOW_WIDTH, WINDOW_HEIGHT),
        car_sprites(window),
        map_manager(window),
        game_hud(window, map_manager, client_id, info_players, car_sprites),
        current_map_id(MapID::LibertyCity),
        time_info(),
        cheat_detector(5) {}

void ClientGame::function() {
    update_state_from_position();

    // Clear display
    window.fill();

    update_animation_frames();

    render_in_z_order();
}

void ClientGame::start() {
    uint8_t map_id = 0;
    while (true) {
        ServerMessage msg = server_handler.recv_response_from_server();
        if (msg.type == ServerMessage::Type::RaceStart) {
            map_id = msg.map_id;
            break;
        } else if (msg.type == ServerMessage::Type::Unknown) {
            std::cout << "[ClientGame] Received Unknown message from server, probably "
                         "disconnected. Exiting..."
                      << std::endl;
            return;
        }
    }

    current_map_id = static_cast<MapID>(map_id);

    map_manager.loadMap(current_map_id);

    std::cout << "[ClientGame] Juego iniciado" << std::endl;

    ConstantRateLoop::start_loop();
}

void ClientGame::handle_cheat_detection(const char* keyName) {
    cheat_detector.add_key(keyName);

    if (cheat_detector.check_cheat("EXIT")) {
        std::cout << "[ClientGame] Cheat code EXIT detected. Exiting game." << std::endl;
        running = false;
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

                // esto se debe borrar cuando se maneje el fin del juego
                game_is_over = true;

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

void ClientGame::update_state_from_position() {
    handle_sdl_events();

    handle_movement_input();

    // Procesar mensajes del servidor
    bool keep_loop = true;
    int msg_count = 0;
    static int frame_count = 0;
    frame_count++;

    while (keep_loop && msg_count < 10) {
        ServerMessage action = server_handler.recv_response_from_server();

        if (action.type == ServerMessage::Type::MapInfo) {
            info_players.clear();

            for (const auto& p_info: action.players_tick) {
                info_players[p_info.player_id] = CarInfoGame{p_info, Area()};
            }

            time_info = action.race_time;
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
            this->running = false;
            std::cout << "[ClientGame] Received Unknown message from server, probably "
                         "disconnected. Exiting..."
                      << std::endl;
        }
        // se debería recibir el fin del juego
        // game_is_over = true;

        msg_count++;
    }
}

void ClientGame::update_map_area() {
    const PlayerTickInfo& info_my_car = info_players[client_id].info_car;

    int x_map = info_my_car.x - MAP_WIDTH_SIZE / 2;
    int y_map = info_my_car.y - MAP_HEIGHT_SIZE / 2;

    if (x_map < 0) {
        x_map = 0;
    }

    if (y_map < 0) {
        y_map = 0;
    }

    if (x_map > map_manager.getCurrentMapWidth() - MAP_WIDTH_SIZE) {
        x_map = map_manager.getCurrentMapWidth() - MAP_WIDTH_SIZE;
    }

    if (y_map > map_manager.getCurrentMapHeight() - MAP_HEIGHT_SIZE) {
        y_map = map_manager.getCurrentMapHeight() - MAP_HEIGHT_SIZE;
    }

    src_area_map.update(x_map, y_map, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);
}

void ClientGame::update_animation_frames() {
    update_map_area();

    Area extend_area_map(src_area_map.getX() - CAR_WIDTH_LARGE,
                         src_area_map.getY() - CAR_HEIGHT_LARGE,
                         src_area_map.getWidth() + CAR_WIDTH_LARGE * 2,
                         src_area_map.getHeight() + CAR_HEIGHT_LARGE * 2);

    for (auto& [id, car]: info_players) {
        if (car.info_car.x < extend_area_map.getX() ||
            car.info_car.x > extend_area_map.getX() + extend_area_map.getWidth() ||
            car.info_car.y < extend_area_map.getY() ||
            car.info_car.y > extend_area_map.getY() + extend_area_map.getHeight()) {
            continue;
        }

        CarData car_data = car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        int x_car_screen = (car.info_car.x - src_area_map.getX()) * MAP_TO_VIEWPORT_SCALE_X;
        int y_car_screen = (car.info_car.y - src_area_map.getY()) * MAP_TO_VIEWPORT_SCALE_Y;
        car.dest_area.update(x_car_screen - car_data.width_scale_screen / 2,
                             y_car_screen - car_data.height_scale_screen / 2,
                             car_data.width_scale_screen, car_data.height_scale_screen);
    }
}

void ClientGame::render_cars() {
    for (const auto& [id, car]: info_players) {
        if (car.dest_area.getWidth() == 0 || car.dest_area.getHeight() == 0) {
            continue;
        }

        const CarData& car_data =
                car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        car_sprites.render(car_data.area, car.dest_area, car.info_car.angle);
    }
}

void ClientGame::render_in_z_order() {
    map_manager.render(src_area_map, dest_area_map);

    render_cars();

    game_hud.render(iteration, time_info.seconds);

    window.render();
}

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

    info_players.clear();
}
