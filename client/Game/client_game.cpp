#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "sdl_wrappers/SdlDrawFill.h"

#include "constants.h"

ClientGame::ClientGame(size_t client_id, ServerHandler& server_handler):
        client_id(client_id),
        server_handler(server_handler),
        running(false),
        src_area_map(0, 0, 0, 0),
        dest_area_map(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
        car_positions() {}

void ClientGame::start() {
    this->running = true;

    // init resources
    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    CarSpriteSheet car_sprites(window);

    LifeBarSpriteSheet life_bar_sprites(window);


    MapsTextures map_manager(window);
    map_manager.loadMap(MapID::LibertyCity);

    std::cout << "[ClientGame] Tamaño real del mapa: " << map_manager.getCurrentMapWidth() << "x"
              << map_manager.getCurrentMapHeight() << std::endl;

    GameHud game_hud(window, map_manager, client_id, car_positions);

    std::cout << "[ClientGame] Juego iniciado, esperando posiciones del servidor..." << std::endl;
    while (this->running) {
        update_state_from_position();

        // Clear display
        window.fill();

        update_animation_frames(map_manager, car_sprites);

        render_in_z_order(window, map_manager, car_sprites, life_bar_sprites, game_hud);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
}

void ClientGame::update_state_from_position() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;
                switch (keyEvent.keysym.sym) {
                    case SDLK_LEFT:
                        server_handler.send_movement(Movement::Left);
                        std::cout << "[ClientGame] LEFT pressed\n";
                        break;
                    case SDLK_RIGHT:
                        server_handler.send_movement(Movement::Right);
                        std::cout << "[ClientGame] RIGHT pressed\n";
                        break;
                    case SDLK_UP:
                        server_handler.send_movement(Movement::Up);
                        std::cout << "[ClientGame] UP pressed\n";
                        break;
                    case SDLK_DOWN:
                        server_handler.send_movement(Movement::Down);
                        std::cout << "[ClientGame] DOWN pressed\n";
                        break;
                }
            } break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_QUIT:
                std::cout << "[ClientGame] Quit event received" << std::endl;
                running = false;
                break;
        }
    }

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

    // Procesar mensajes del servidor
    bool keep_loop = true;
    int msg_count = 0;
    static int frame_count = 0;
    frame_count++;

    while (keep_loop && msg_count < 10) {
        ServerMessage action = server_handler.recv_response_from_server();

        if (action.type == ServerMessage::Type::Pos) {
            car_positions.clear();

            std::cout << "[ClientGame] Posición recibida del servidor: (" << action.x << ", "
                      << action.y << ", " << action.angle << ")\n";

            car_positions[action.id] = {
                    {static_cast<int>(action.x), static_cast<int>(action.y), action.angle},
                    Area(0, 0, 0, 0)};
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
        }
        msg_count++;
    }
}

void ClientGame::update_map_area(const MapsTextures& map_manager) {
    const Position& position_my_car = car_positions[client_id].position;

    int x_map = position_my_car.x_car_map - MAP_WIDTH_SIZE / 2;
    int y_map = position_my_car.y_car_map - MAP_HEIGHT_SIZE / 2;

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

void ClientGame::update_animation_frames(const MapsTextures& map_manager,
                                         const CarSpriteSheet& car_sprites) {
    update_map_area(map_manager);

    Area extend_area_map(src_area_map.getX() - CAR_WIDTH_LARGE,
                         src_area_map.getY() - CAR_HEIGHT_LARGE,
                         src_area_map.getWidth() + CAR_WIDTH_LARGE * 2,
                         src_area_map.getHeight() + CAR_HEIGHT_LARGE * 2);

    for (auto& [id, car_pos]: car_positions) {
        if (car_pos.position.x_car_map < extend_area_map.getX() ||
            car_pos.position.x_car_map > extend_area_map.getX() + extend_area_map.getWidth() ||
            car_pos.position.y_car_map < extend_area_map.getY() ||
            car_pos.position.y_car_map > extend_area_map.getY() + extend_area_map.getHeight()) {
            continue;
        }

        // el auto debería de pasarlo el server en algún momento
        CarData car_data = car_sprites.getCarData(this->current_car);

        int x_car_screen =
                (car_pos.position.x_car_map - src_area_map.getX()) * MAP_TO_VIEWPORT_SCALE_X;
        int y_car_screen =
                (car_pos.position.y_car_map - src_area_map.getY()) * MAP_TO_VIEWPORT_SCALE_Y;
        car_pos.dest_area.update(x_car_screen - car_data.width_scale_screen / 2,
                                 y_car_screen - car_data.height_scale_screen / 2,
                                 car_data.width_scale_screen, car_data.height_scale_screen);
    }
}

void ClientGame::render_cars(const CarSpriteSheet& car_sprites,
                             const LifeBarSpriteSheet& life_bar_sprites) {
    for (const auto& [id, car_pos]: car_positions) {
        if (car_pos.dest_area.getWidth() == 0 || car_pos.dest_area.getHeight() == 0) {
            continue;
        }

        const CarData& car_data = car_sprites.getCarData(this->current_car);
        life_bar_sprites.render(100, 5,
                                Area(car_pos.dest_area.getX(),
                                     car_pos.dest_area.getY() - car_data.width_scale_screen / 5,
                                     car_data.width_scale_screen, car_data.width_scale_screen / 5));
        // el auto debería de pasarlo el server en algún momento
        car_sprites.render(car_data.area, car_pos.dest_area, car_pos.position.angle);
    }
}

void ClientGame::render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                                   const CarSpriteSheet& car_sprites,
                                   const LifeBarSpriteSheet& life_bar_sprites, GameHud& game_hud) {

    map_manager.render(src_area_map, dest_area_map);

    render_cars(car_sprites, life_bar_sprites);

    game_hud.render();

    window.render();
}

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

    car_positions.clear();
}
