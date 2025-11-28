#ifndef CLIENT_CONSTANTS_H
#define CLIENT_CONSTANTS_H

#include <cstdint>
#include <string_view>

constexpr int BACKGROUND_COLOR_R = 163;
constexpr int BACKGROUND_COLOR_G = 163;
constexpr int BACKGROUND_COLOR_B = 13;

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

constexpr int MAP_WIDTH_SIZE = 720;
constexpr int MAP_HEIGHT_SIZE = 405;

constexpr float MAP_TO_VIEWPORT_SCALE_X =
        static_cast<float>(WINDOW_WIDTH) / MAP_WIDTH_SIZE;  // 2.6667
constexpr float MAP_TO_VIEWPORT_SCALE_Y =
        static_cast<float>(WINDOW_HEIGHT) / MAP_HEIGHT_SIZE;  // 2.6667

// Car size in relation to the screen
constexpr int CAR_WIDTH_SMALL = 80;
constexpr int CAR_HEIGHT_SMALL = 80;
constexpr int CAR_WIDTH_MEDIUM = 100;
constexpr int CAR_HEIGHT_MEDIUM = 100;
constexpr int CAR_WIDTH_LARGE = 120;
constexpr int CAR_HEIGHT_LARGE = 120;

// font path
#define FONT_STYLE_AA std::string(ASSETS_PATH) + "/font/AldotheApache.ttf"
#define FONT_STYLE_PX std::string(ASSETS_PATH) + "/font/Pixeboy-z8XGD.ttf"
#define FONT_STYLE_PS std::string(ASSETS_PATH) + "/font/PrStart.ttf"
#define FONT_STYLE_CC std::string(ASSETS_PATH) + "/font/C&C Red Alert [INET].ttf"
#define FONT_STYLE_VS1 std::string(ASSETS_PATH) + "/font/visitor1.ttf"
#define FONT_STYLE_VS2 std::string(ASSETS_PATH) + "/font/visitor2.ttf"

#define FRAME_RATE 60.0

#define RGB_BACKGROUND Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)

#endif  // CONSTANTS_H
