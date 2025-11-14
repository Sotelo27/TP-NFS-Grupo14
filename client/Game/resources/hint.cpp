#include "hint.h"

#include <cmath>

#include "../constants.h"
#include "../utils/rgb.h"

#define HINT_IMAGE_PATH std::string(ASSETS_PATH) + "/hud/flecha.png"

Hint::Hint(const SdlWindow& window):
        texture(HINT_IMAGE_PATH, window,
                Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)) {}

void Hint::render(int x_car, int y_car, uint distance_checkpoint, double angle, int iteration,
                  int car_width_scale_screen, int car_height_scale_screen) const {
    iteration = distance_checkpoint;
    distance_checkpoint = iteration;

    // se recupera la verdadera posicion del auto en el mapa
    x_car += car_width_scale_screen / 2;
    y_car += car_height_scale_screen / 2;
    Area dest(x_car - texture.getWidth() / 2, y_car - texture.getHeight() / 2, texture.getWidth(),
              texture.getHeight());
    texture.renderEntity(Area(0, 0, texture.getWidth(), texture.getHeight()), dest, angle);
}
