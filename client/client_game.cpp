#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "constants.h"

ClientGame::ClientGame(CarSpriteID car, size_t client_id, const char* host, const char* service):
        current_car(car),
        client_id(client_id),
        server_actions{},
        server_handler(std::move(Socket(host, service)), server_actions),
        running(false),
        positions(),
        src_area_map(0, 0, 0, 0),
        dest_area_map(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
        map_dest_areas() {}

void ClientGame::start() {
    server_handler.start();

    this->running = true;

    // init resources
    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    CarSpriteSheet car_sprites(window);

    MapsTextures map_manager(window);
    map_manager.loadMap(MapID::LibertyCity);

    const MapData& map_data = map_manager.getCurrentMapData();
    std::cout << "Tamaño real del mapa: " << map_data.width_scale_screen << "x"
              << map_data.height_scale_screen << std::endl;

    while (this->running) {
        update_state_from_position();

        // Clear display
        window.fill();

        update_animation_frames(map_data, car_sprites);

        render_in_z_order(window, map_manager, car_sprites);
    }
}

void ClientGame::update_state_from_position() {
    SDL_Event event;

    // Para el alumno: Buscar diferencia entre waitEvent y pollEvent
    SDL_WaitEvent(&event);
    switch (event.type) {
        case SDL_KEYDOWN: {
            const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;
            switch (keyEvent.keysym.sym) {
                case SDLK_LEFT:
                    server_handler.send_movement(Movement::Left);
                    std::cout << "Left key pressed, send action to server." << std::endl;
                    break;
                case SDLK_RIGHT:
                    server_handler.send_movement(Movement::Right);
                    std::cout << "Right key pressed, send action to server." << std::endl;
                    break;
                case SDLK_UP:
                    server_handler.send_movement(Movement::Up);
                    std::cout << "Up key pressed, send action to server." << std::endl;
                    break;
                case SDLK_DOWN:
                    server_handler.send_movement(Movement::Down);
                    std::cout << "Down key pressed, send action to server." << std::endl;
                    break;
            }
        }  // Fin KEY_DOWN
        break;
        case SDL_MOUSEMOTION:
            break;
        case SDL_QUIT:
            std::cout << "Quit :(" << std::endl;
            running = false;
            break;
    }

    ServerMessage action;
    while (server_actions.try_pop(action)) {
        try {
            if (action.type == ServerMessage::Type::Pos) {
                std::cout << "Received position update from server: (" << action.x << ", "
                          << action.y << ")\n";
                positions.x_car_map = action.x;
                positions.y_car_map = action.y;
            }

        } catch (const std::exception& err) {
            std::cerr << "Error processing action from server " << action.id << ": " << err.what()
                      << "\n";
        }
    }
}

void ClientGame::update_animation_frames(const MapData& map_data,
                                         const CarSpriteSheet& car_sprites) {
    int x_map = positions.x_car_map - MAP_WIDTH_SIZE / 2;
    int y_map = positions.y_car_map - MAP_HEIGHT_SIZE / 2;

    int x_car_screen = WINDOW_WIDTH / 2;
    int y_car_screen = WINDOW_HEIGHT / 2;

    if (x_map < 0) {
        x_car_screen = positions.x_car_map * MAP_TO_VIEWPORT_SCALE_X;
        x_map = 0;
    }

    if (y_map < 0) {
        y_car_screen = positions.y_car_map * MAP_TO_VIEWPORT_SCALE_Y;
        y_map = 0;
    }

    if (x_map > map_data.width_scale_screen - MAP_WIDTH_SIZE) {
        x_map = map_data.width_scale_screen - MAP_WIDTH_SIZE;
        x_car_screen = (positions.x_car_map - x_map) * MAP_TO_VIEWPORT_SCALE_X;
    }

    if (y_map > map_data.height_scale_screen - MAP_HEIGHT_SIZE) {
        y_map = map_data.height_scale_screen - MAP_HEIGHT_SIZE;
        y_car_screen = (positions.y_car_map - y_map) * MAP_TO_VIEWPORT_SCALE_Y;
    }

    src_area_map.update(x_map, y_map, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);

    CarData car_data = car_sprites.getCarData(this->current_car);
    map_dest_areas[client_id] = Area(x_car_screen - car_data.width_scale_screen / 2,
                                     y_car_screen - car_data.height_scale_screen / 2,
                                     car_data.width_scale_screen, car_data.height_scale_screen);
}

void ClientGame::render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                                   const CarSpriteSheet& car_sprites) {
    const CarData& car_data = car_sprites.getCarData(this->current_car);

    map_manager.render(src_area_map, dest_area_map);
    car_sprites.render(car_data.area, map_dest_areas[client_id]);

    window.render();
}

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

    map_dest_areas.clear();
}
