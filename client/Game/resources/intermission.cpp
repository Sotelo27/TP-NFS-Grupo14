#include "intermission.h"

#include "../constants.h"
#include "../utils/rgb.h"

#define BACKGROUND_IMAGE_PATH std::string(ASSETS_PATH) + "/images/fondo_cars.jpg"
#define SIZE (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 27.27
#define AMOUNT_FRAMES_ANIMATION 90

Intermission::Intermission(const SdlWindow& window):
        active(false),
        iterations(1),
        background_texture(BACKGROUND_IMAGE_PATH, window, Rgb(0, 255, 0)),
        text(FONT_STYLE_PX, SIZE, window) {}

void Intermission::render() {
    std::list<PlayerInfoI> dummy; // en un futuro se recibe la lista de jugadores
    dummy.push_back({1, "Player1", 120, 300});
    dummy.push_back({2, "Player2", 150, 320});
    dummy.push_back({3, "Player3", 180, 350});  
    dummy.push_back({4, "Player4", 200, 400});
    dummy.push_back({5, "Player5", 220, 450});
    dummy.push_back({6, "Player6", 250, 500});
    dummy.push_back({7, "Player7", 300, 600});
    dummy.push_back({8, "Player8", 350, 700});

    if (!active) {
        active = true;
        iterations = 1; // esto en un futuro se usa el iteration del bucle principal
    }

    if (iterations <= AMOUNT_FRAMES_ANIMATION) {
        int y_animation = (background_texture.getHeight() * iterations) / AMOUNT_FRAMES_ANIMATION;
        int y_window = (WINDOW_HEIGHT * iterations) / AMOUNT_FRAMES_ANIMATION;
        background_texture.renderEntity(
                Area(0, y_animation, background_texture.getWidth(), background_texture.getHeight()),
                Area(0, 0, WINDOW_WIDTH, y_window), 0.0);
    } else if (iterations > AMOUNT_FRAMES_ANIMATION + 30) {
        text.renderDirect(SIZE, SIZE, "POSITion 123 45 67890", Rgb(0, 255, 0), true, Rgb(255, 0, 0));
    }
    iterations++; // esto se saca cuando se use el del bucle principal
}

void Intermission::reset() {
    active = false;
}
