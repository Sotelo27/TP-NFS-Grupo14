#include "life_hud.h"

#include "../../constants.h"
#include "../../utils/rgb.h"

#define SIZE WINDOW_WIDTH / 43

#define LIFE_ICON std::string(ASSETS_PATH) + "/hud/vida.png"

LifeHud::LifeHud(const SdlWindow& window):
        texture(LIFE_ICON, window, RGB_BACKGROUND), text(FONT_STYLE_AA, (SIZE * 11) / 12, window) {}

void LifeHud::render(int max_life, int current_life, int x, int y) {
    this->texture.render(Area(0, 0, texture.getWidth(), texture.getHeight()),
                         Area(x, y, SIZE, SIZE * texture.getHeight() / texture.getWidth()));

    int g = (max_life <= 0 || current_life <= 0) ? 0 : (255 * current_life) / max_life;
    int r = 255 - g;

    text.renderDirect(x + SIZE + 5, y + SIZE / 12, std::to_string(current_life), Rgb(r, g, 0),
                      true);
}
