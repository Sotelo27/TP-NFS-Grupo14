#include "speed_hud.h"

#include <algorithm>

#include "../constants.h"
#include "../utils/rgb.h"

#define SIZE WINDOW_WIDTH / 8
#define SIZE_FONT_NUMBER (SIZE * 22) / 48
#define SIZE_FONT_UNIT (SIZE * 10) / 48

SpeedHud::SpeedHud(const SdlWindow& window):
        textNumber(FONT_STYLE_AA, SIZE_FONT_NUMBER, window),
        textUnit(FONT_STYLE_AA, SIZE_FONT_UNIT, window) {}

void SpeedHud::render(int speed, int x, int y) {
    speed = std::max(0, speed);

    Rgb white(255, 255, 255);
    Rgb white_semi(255, 255, 255, 150);

    int x_km = x + SIZE - SIZE_FONT_UNIT - SIZE_FONT_UNIT / 3 - 5;
    int y_km = y + SIZE / 12 + 12;

    textUnit.renderDirect(x_km, y_km, "km", white, true, white_semi);

    textUnit.renderDirect(x_km, y_km + 5, "__", white, true, white);

    textUnit.renderDirect(x_km + SIZE_FONT_UNIT / 4 + 2, y_km + (SIZE_FONT_UNIT * 9) / 8, "h",
                          white, true, white_semi);

    std::string speed_str = std::to_string(speed);
    textNumber.loadText(speed_str, white, true);
    int x_speed = x_km - textNumber.getWidth() - 7;

    textNumber.renderDirect(x_speed, y_km + 7, speed_str, white, true, white_semi);
}
