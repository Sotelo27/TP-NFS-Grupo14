#include "hint.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdint>
#include <iostream>

#include "../../constants.h"
#include "../../utils/rgb.h"

#define HINT_IMAGE_PATH std::string(ASSETS_PATH) + "/hud/hint.png"

#define CLOSENESS_FACTOR 10
#define MAX_RANGE_CHECKPOINT 1500
#define ARROW_RANGE_FACTOR (static_cast<float>(MAX_RANGE_CHECKPOINT) / 425)

#define MIN_FACTOR_ACHICAR 0.53
#define REDUCE_FACTOR_IMAGE 17.92

#define NUMBER_ARROWS 5

Hint::Hint(const SdlWindow& window):
        texture(HINT_IMAGE_PATH, window,
                Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        phase(0) {}

void Hint::render(int x_car, int y_car, int distance_checkpoint, double angle, int iteration,
                  int car_width_scale_screen, int car_height_scale_screen) {
    distance_checkpoint = std::min(MAX_RANGE_CHECKPOINT, distance_checkpoint);
    int distance_arrow = static_cast<float>(distance_checkpoint) /
                         ARROW_RANGE_FACTOR;  // rango de aparicion de la flecha

    // se recupera la verdadera posicion del auto en el mapa
    x_car += car_width_scale_screen / 2;
    y_car += car_height_scale_screen / 2;

    for (uint i = 0; i < NUMBER_ARROWS; i++) {
        adjustAlphaForAnimation(i);

        int current_x_arrow = x_car;
        int current_y_arrow = y_car;
        // se mueve la flecha hacia la posicion del checkpoint
        float angle_rad = angle * M_PI / 180.0f;  // a radianes
        current_x_arrow += std::cos(angle_rad) *
                           ((distance_arrow * (CLOSENESS_FACTOR - i)) / CLOSENESS_FACTOR);
        current_y_arrow += std::sin(angle_rad) *
                           ((distance_arrow * (CLOSENESS_FACTOR - i)) / CLOSENESS_FACTOR);

        float t = static_cast<float>(distance_checkpoint) / MAX_RANGE_CHECKPOINT;
        // float arrow_size_adjustment_factor = std::cbrt(t);
        float arrow_size_adjustment_factor = std::sqrt(t);

        int max_hint_width = static_cast<float>(texture.getWidth()) / REDUCE_FACTOR_IMAGE;
        int max_hint_height = static_cast<float>(texture.getHeight()) / REDUCE_FACTOR_IMAGE;

        float offset_img_width = max_hint_width * arrow_size_adjustment_factor / 2;
        float offset_img_height = max_hint_height * arrow_size_adjustment_factor / 2;
        Area dest(current_x_arrow - offset_img_width, current_y_arrow - offset_img_height,
                  max_hint_width * arrow_size_adjustment_factor,
                  max_hint_height * arrow_size_adjustment_factor);
        texture.renderEntity(Area(0, 0, texture.getWidth(), texture.getHeight()), dest, angle);
    }

    if (iteration % 10 == 0) {
        phase++;
    }
    if (phase >= NUMBER_ARROWS) {
        phase = 0;
    }
}

void Hint::adjustAlphaForAnimation(int arrow_index) {
    std::vector<uint8_t> alphas = {255, 200, 150, 100, 50};
    int alpha_index = arrow_index - phase;
    if (alpha_index > 0) {
        alpha_index = NUMBER_ARROWS - alpha_index;
    } else if (alpha_index < 0) {
        alpha_index = -alpha_index;
    }

    texture.changeAlpha(alphas[alpha_index]);
}
