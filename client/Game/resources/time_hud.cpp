#include "time_hud.h"

#include <iomanip>
#include <sstream>

#include "../constants.h"

#define TIME_ICON std::string(ASSETS_PATH) + "/hud/tiempo.png"

#define SIZE WINDOW_WIDTH / 38

TimeHud::TimeHud(const SdlWindow& window):
        texture(TIME_ICON, window, Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        text(FONT_STYLE_PX, SIZE + 10, window) {}

void TimeHud::render(int16_t time_seconds, int x, int y) {
    time_seconds = (time_seconds < 0) ? 0 : time_seconds;

    int16_t minutes = time_seconds / 60;
    int16_t seconds = time_seconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << std::abs(minutes) << ":" << std::setfill('0')
       << std::setw(2) << std::abs(seconds);
    std::string buffer = ss.str();

    int text_x = x - text.getWidth() / 2;
    text.renderDirect(text_x, y + SIZE / 5, buffer, Rgb(255, 255, 255), true);

    this->texture.render(Area(0, 0, texture.getWidth(), texture.getHeight()),
                         Area(text_x - SIZE - 5, y, SIZE, SIZE * texture.getHeight() / texture.getWidth()));
}
