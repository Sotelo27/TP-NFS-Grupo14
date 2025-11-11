#include "speed_hud.h"

#include "../constants.h"
#include "../utils/rgb.h"

#define SIZE WINDOW_WIDTH / 10

#define SPEED_BACKGROUND std::string(ASSETS_PATH) + "/images/fondo_speed2.png"
#define FONT_STYLE std::string(ASSETS_PATH) + "/font/AldotheApache.ttf"

SpeedHud::SpeedHud(const SdlWindow& window):
        texture(SPEED_BACKGROUND, window, Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        text(FONT_STYLE, (SIZE * 11) / 12, window) {}

void SpeedHud::render(int speed, int x, int y) const {
    this->texture.render(Area(63, 2840, 1542, 731),
                         Area(x, y, SIZE, SIZE * texture.getHeight() / texture.getWidth()));

    speed = std::max(0, speed);

//     text.renderText(std::to_string(speed), Rgb(r, g, 0),
//                     Area(x + SIZE + 5, y + SIZE / 12, SIZE * 3, SIZE));
}
