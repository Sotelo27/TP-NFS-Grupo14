#include "time_hud.h"

#include <iomanip>
#include <sstream>

#include "../../constants.h"
#include "../../utils/time_formatter.h"

#define TIME_ICON std::string(ASSETS_PATH) + "/hud/tiempo.png"

#define SIZE WINDOW_WIDTH / 38

TimeHud::TimeHud(const SdlWindow& window):
        texture(TIME_ICON, window, RGB_BACKGROUND), text(FONT_STYLE_PX, SIZE + 10, window) {}

void TimeHud::render(int16_t time_seconds, int x, int y) {
    std::string buffer = TimeFormatter::format_time(time_seconds);

    int text_x = x - text.getWidth() / 2;
    text.renderDirect(text_x, y + SIZE / 5, buffer, Rgb(255, 255, 255), true);

    this->texture.render(
            Area(0, 0, texture.getWidth(), texture.getHeight()),
            Area(text_x - SIZE - 5, y, SIZE, SIZE * texture.getHeight() / texture.getWidth()));
}
