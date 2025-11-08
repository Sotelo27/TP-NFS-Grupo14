#include "life.h"

#include "../constants.h"
#include "../utils/rgb.h"

#define SIZE WINDOW_WIDTH / 32

#define LIFE_ICON std::string(ASSETS_PATH) + "/images/vida.png"
#define FONT_STYLE std::string(ASSETS_PATH) + "/font/PrStart.ttf"

LifeHud::LifeHud(const SdlWindow& window):
        texture(LIFE_ICON, window, Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        text(FONT_STYLE, (SIZE * 7) / 12, window) {}

void LifeHud::render(int max_life, int current_life, int x, int y) const {
    this->texture.render(Area(0, 0, texture.getWidth(), texture.getHeight()),
                         Area(x, y, SIZE, SIZE * texture.getHeight() / texture.getWidth()));

    int g = (255 * current_life) / max_life;
    int r = 255 - g;

    text.renderText(std::to_string(current_life), Rgb(r, g, 0),
                    Area(x + SIZE + 5, y + SIZE / 6, SIZE * 2, SIZE));
}
