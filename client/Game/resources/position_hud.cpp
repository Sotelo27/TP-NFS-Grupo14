#include "position_hud.h"

#include "../constants.h"
#include "../utils/rgb.h"

PositionHud::PositionHud(const SdlWindow& window):
        text(FONT_STYLE_AA, 60, window) {}

void PositionHud::render(int position, int x, int y) {
    std::string position_str = getOrdinalString(position);

    text.renderDirect(x, y, position_str, Rgb(255, 255, 255), true);
}

std::string PositionHud::getOrdinalSuffix(int number) {
    int last_two = number % 100;
    if (last_two >= 11 && last_two <= 13) {
        return "th";
    }

    switch (number % 10) {
        case 1:
            return "st";
        case 2:
            return "nd";
        case 3:
            return "rd";
        default:
            return "th";
    }
}

std::string PositionHud::getOrdinalString(int number) {
    return std::to_string(number) + getOrdinalSuffix(number);
}
