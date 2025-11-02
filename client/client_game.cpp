#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "sdl_wrappers/SdlWindow.h"
#include "utils/maps_textures.h"

#include "constants.h"

ClientGame::ClientGame(CarSpriteID car, const char* host, const char* service):
        current_car(car),
        server_actions{},
        server_handler(std::move(Socket(host, service)), server_actions),
        running(false),
        positions() {}

void ClientGame::start() {
    server_handler.start();

    this->running = true;

    // init resources
    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    CarSpriteSheet car_sprites(window);
    const CarData& car_data = car_sprites.getCarData(this->current_car);

    MapsTextures map_manager(window);
    map_manager.loadMap(MapID::LibertyCity);

    const MapData& map_data = map_manager.getCurrentMapData();
    std::cout << "Tamaño real del mapa: " << map_data.width_scale_screen << "x"
              << map_data.height_scale_screen << std::endl;

    while (this->running) {
        update_state_from_position();
        //Clear display
        window.fill();

        // Calcular la región del mapa a mostrar (centrada en el auto)
        int x_map = positions.x_car_map - MAP_WIDTH_SIZE / 2;
        int y_map = positions.y_car_map - MAP_HEIGHT_SIZE / 2;

        // Por defecto, el auto está en el centro de la pantalla
        int x_car_screen = WINDOW_WIDTH / 2;
        int y_car_screen = WINDOW_HEIGHT / 2;

        // Ajustar cuando llegamos al borde izquierdo
        if (x_map < 0) {
            x_car_screen = positions.x_car_map * MAP_TO_VIEWPORT_SCALE_X;
            x_map = 0;
        }

        // Ajustar cuando llegamos al borde superior
        if (y_map < 0) {
            y_car_screen = positions.y_car_map * MAP_TO_VIEWPORT_SCALE_Y;
            y_map = 0;
        }

        // Ajustar cuando llegamos al borde derecho
        if (x_map > map_data.width_scale_screen - MAP_WIDTH_SIZE) {
            x_map = map_data.width_scale_screen - MAP_WIDTH_SIZE;
            x_car_screen = (positions.x_car_map - x_map) * MAP_TO_VIEWPORT_SCALE_X;
        }

        // Ajustar cuando llegamos al borde inferior
        if (y_map > map_data.height_scale_screen - MAP_HEIGHT_SIZE) {
            y_map = map_data.height_scale_screen - MAP_HEIGHT_SIZE;
            y_car_screen = (positions.y_car_map - y_map) * MAP_TO_VIEWPORT_SCALE_Y;
        }

        Area srcArea(x_map, y_map, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);
        Area destArea(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        Area destAreaCar(x_car_screen - car_data.width_scale_screen / 2,
                         y_car_screen - car_data.height_scale_screen / 2,
                         car_data.width_scale_screen, car_data.height_scale_screen);

        map_manager.render(srcArea, destArea);
        car_sprites.render(car_data.area, destAreaCar);
        window.render();
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

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }
}
