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

ClientGame::ClientGame(CarSpriteID car): running(false), current_car(car) {}

void ClientGame::start() {
    this->running = true;
    // init resources
    Rgb background_color(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B);
    CarSpriteSheet car_sprites;
    const CarData& car_data = car_sprites.get(this->current_car);

    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    SdlMapTexture im(ASSETS + "/cities/Liberty City.png", window);
    SdlObjTexture auto1(ASSETS + "/cars/Cars.png", window, background_color);

    int x_car_map = 1900;
    int y_car_map = 1900;

    int x_car_screen = WINDOW_WIDTH / 2 - car_data.width_scale_screen / 2;
    int y_car_screen = WINDOW_HEIGHT / 2 - car_data.height_scale_screen / 2;
    while (this->running) {
        SDL_Event event;

        // Para el alumno: Buscar diferencia entre waitEvent y pollEvent
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;
                switch (keyEvent.keysym.sym) {
                    case SDLK_LEFT:
                        x_car_map -= 10;
                        break;
                    case SDLK_RIGHT:
                        x_car_map += 10;
                        break;
                    case SDLK_UP:
                        y_car_map -= 10;
                        break;
                    case SDLK_DOWN:
                        y_car_map += 10;
                        break;
                }
            }  // Fin KEY_DOWN
            break;
            case SDL_MOUSEMOTION:
                std::cout << "Oh! Mouse" << std::endl;
                break;
            case SDL_QUIT:
                std::cout << "Quit :(" << std::endl;
                running = false;
                break;
        }

        window.fill();  // Repinto el fondo gris

        Area srcArea(x_car_map - MAP_WIDTH_SIZE / 2, y_car_map - MAP_HEIGHT_SIZE / 2,
                     MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);
        Area destArea(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        Area destAreaCar(x_car_screen, y_car_screen, car_data.width_scale_screen,
                         car_data.height_scale_screen);

        im.render(srcArea, destArea);
        auto1.render(car_data.area, destAreaCar);

        window.render();
    }
}
