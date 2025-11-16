#include "intermission.h"

#include "../constants.h"
#include "../utils/rgb.h"

#define BACKGROUND_IMAGE_PATH std::string(ASSETS_PATH) + "/images/fondo_cars.jpg"

#define AMOUNT_FRAMES_ANIMATION 120

Intermission::Intermission(const SdlWindow& window):
        active(false),
        iterations(1),
        background_texture(BACKGROUND_IMAGE_PATH, window, Rgb(0, 255, 0)) {}

void Intermission::render() {
    if (!active) {
        active = true;
        iterations = 1;
    }

    if (iterations <= AMOUNT_FRAMES_ANIMATION) {
        int y_animation = (background_texture.getHeight() * iterations) / AMOUNT_FRAMES_ANIMATION;
        int y_window = (WINDOW_HEIGHT * iterations) / AMOUNT_FRAMES_ANIMATION;
        background_texture.renderEntity(
                Area(0, background_texture.getHeight() - y_animation, background_texture.getWidth(), background_texture.getHeight()),
                Area(0, 0, WINDOW_WIDTH, y_window), 0.0);
        iterations++;
    }
}
