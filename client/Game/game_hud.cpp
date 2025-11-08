#include "game_hud.h"

#include "sdl_wrappers/SdlDrawFill.h"
#include "sdl_wrappers/SdlObjTexture.h"

#include "constants.h"

#define POINT_RED std::string(ASSETS_PATH) + "/images/punto_rojo.png"

#define FONT_STYLE std::string(ASSETS_PATH) + "/font/PrStart.ttf"

GameHud::GameHud(const SdlWindow& window, const MapsTextures& map_manager, size_t client_id,
                 std::unordered_map<size_t, CarPosition>& car_positions):
        window(window),
        map_manager(map_manager),
        client_id(client_id),
        car_positions(car_positions),
        life_hud(window) {}

void GameHud::renderMiniMapBorder(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                                  int mini_map_height) {
    int border = 4;
    Rgb color(0, 0, 0, 255);

    Area top(x_dest_mini_map - border, y_dest_mini_map - border, mini_map_width + border * 2,
             border);
    Area bottom(x_dest_mini_map - border, y_dest_mini_map + mini_map_height,
                mini_map_width + border * 2, border);
    Area left(x_dest_mini_map - border, y_dest_mini_map, border, mini_map_height);
    Area right(x_dest_mini_map + mini_map_width, y_dest_mini_map, border, mini_map_height);

    SdlDrawFill draw_fill(window);
    draw_fill.fill(top, color);
    draw_fill.fill(bottom, color);
    draw_fill.fill(left, color);
    draw_fill.fill(right, color);
}

void GameHud::renderPositionMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                                    int mini_map_height) {
    const Position& position_my_car = car_positions[client_id].position;
    int x_car_mini_map = x_dest_mini_map + (position_my_car.x_car_map * mini_map_width) /
                                                   map_manager.getCurrentMapWidth();
    int y_car_mini_map = y_dest_mini_map + (position_my_car.y_car_map * mini_map_height) /
                                                   map_manager.getCurrentMapHeight();

    Area car_area_mini_map(x_car_mini_map - 5, y_car_mini_map - 5, 10, 10);
    SdlObjTexture car_mini_map(POINT_RED, window, Rgb(0, 0, 0));
    car_mini_map.renderEntity(Area(0, 0, car_mini_map.getWidth(), car_mini_map.getHeight()),
                              car_area_mini_map, position_my_car.angle);
}

void GameHud::renderMiniMap() {
    int y_dest_mini_map = 15;
    int x_dest_mini_map = WINDOW_WIDTH - (MAP_HEIGHT_SIZE * 3 / 4) - y_dest_mini_map;
    int mini_map_width = 300;
    int mini_map_height =
            mini_map_width * map_manager.getCurrentMapHeight() / map_manager.getCurrentMapWidth();

    Area src_mini_map_area(0, 0, map_manager.getCurrentMapWidth(),
                           map_manager.getCurrentMapHeight());
    Area dest_mini_map_area(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);
    map_manager.render(src_mini_map_area, dest_mini_map_area);

    renderMiniMapBorder(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);

    renderPositionMiniMap(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);
}

void GameHud::render() {
    life_hud.render(100, 75, 15, 15);

    renderMiniMap();
}
