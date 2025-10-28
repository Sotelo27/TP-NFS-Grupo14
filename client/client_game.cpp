#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "sdl_wrappers/SdlMapTexture.h"
#include "sdl_wrappers/SdlObjTexture.h"
#include "sdl_wrappers/SdlWindow.h"

#include "constants.h"

#define ASSETS std::string(ASSETS_PATH)

ClientGame::ClientGame(CarSpriteID car, const char* host, const char* service):
        current_car(car),
        server_actions{},
        server_handler(std::move(Socket(host, service)), server_actions),
        running(false) {}

void ClientGame::start() {
    server_handler.start();

    this->running = true;
    // init resources
    Rgb background_color(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B);
    CarSpriteSheet car_sprites;
    const CarData& car_data = car_sprites.get(this->current_car);

    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    SdlMapTexture im(ASSETS + "/cities/Liberty City.png", window);
    SdlObjTexture auto1(ASSETS + "/cars/Cars.png", window, background_color);

    positions.x_car_map = 1900;
    positions.y_car_map = 1900;

    positions.x_car_screen = WINDOW_WIDTH / 2 - car_data.width_scale_screen / 2;
    positions.y_car_screen = WINDOW_HEIGHT / 2 - car_data.height_scale_screen / 2;
    while (this->running) {
        update_position();

        window.fill();  // Repinto el fondo gris

        Area srcArea(positions.x_car_map - MAP_WIDTH_SIZE / 2,
                     positions.y_car_map - MAP_HEIGHT_SIZE / 2, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);
        Area destArea(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        Area destAreaCar(positions.x_car_screen, positions.y_car_screen,
                         car_data.width_scale_screen, car_data.height_scale_screen);

        im.render(srcArea, destArea);
        auto1.render(car_data.area, destAreaCar);

        window.render();
    }
}

void ClientGame::update_position() {
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
