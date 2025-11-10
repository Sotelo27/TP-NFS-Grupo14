#include "time_hud.h"

#include <iomanip>
#include <sstream>

#include "../constants.h"

//#define FONT_STYLE std::string(ASSETS_PATH) + "/font/AldotheApache.ttf"
//#define FONT_STYLE std::string(ASSETS_PATH) + "/font/PrStart.ttf"
#define FONT_STYLE std::string(ASSETS_PATH) + "/font/Pixeboy-z8XGD.ttf"

#define TIME_ICON std::string(ASSETS_PATH) + "/images/tiempo2.png"

#define SIZE WINDOW_WIDTH / 38

TimeHud::TimeHud(const SdlWindow& window):
        texture(TIME_ICON, window, Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        text(FONT_STYLE, SIZE + 10, window) {}

void TimeHud::render(int16_t time_seconds, int x, int y) const {
    this->texture.render(Area(0, 0, texture.getWidth(), texture.getHeight()),
                         Area(x, y, SIZE, SIZE * texture.getHeight() / texture.getWidth()));

    int16_t minutes = time_seconds / 60;
    int16_t seconds = time_seconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << std::abs(minutes) << ":" << std::setfill('0')
       << std::setw(2) << std::abs(seconds);
    std::string buffer = ss.str();

    text.renderText(buffer, Rgb(255, 255, 255), Area(x + SIZE + 5, y + SIZE / 5, SIZE * 3, SIZE));
}
