#include "mini_map_hud.h"

#define POINT_RED std::string(ASSETS_PATH) + "/images/punto_rojo.png"

MiniMap::MiniMap(size_t client_id, const SdlWindow& window, const MapsTextures& map_manager,
                 std::unordered_map<size_t, CarInfoGame>& info_players):
        texture_cars_mini_map(POINT_RED, window, Rgb(0, 0, 0)),
        draw_fill(window),
        client_id(client_id),
        map_manager(map_manager),
        info_players(info_players) {}

void MiniMap::render(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                     int mini_map_height) {
    Area src_mini_map_area(0, 0, map_manager.getCurrentMapWidth(),
                           map_manager.getCurrentMapHeight());
    Area dest_mini_map_area(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);
    map_manager.render(src_mini_map_area, dest_mini_map_area);

    renderMiniMapBorder(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);

    renderPositionMiniMap(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);
}

void MiniMap::renderMiniMapBorder(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                                  int mini_map_height) {
    int border = 4;
    Rgb color(0, 0, 0, 255);

    Area top(x_dest_mini_map - border, y_dest_mini_map - border, mini_map_width + border * 2,
             border);
    Area bottom(x_dest_mini_map - border, y_dest_mini_map + mini_map_height,
                mini_map_width + border * 2, border);
    Area left(x_dest_mini_map - border, y_dest_mini_map, border, mini_map_height);
    Area right(x_dest_mini_map + mini_map_width, y_dest_mini_map, border, mini_map_height);

    draw_fill.fill(top, color);
    draw_fill.fill(bottom, color);
    draw_fill.fill(left, color);
    draw_fill.fill(right, color);
}

void MiniMap::renderPositionMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                                    int mini_map_height) {
    const PlayerTickInfo& info_my_car = info_players[client_id].info_car;
    int x_car_mini_map =
            x_dest_mini_map + (info_my_car.x * mini_map_width) / map_manager.getCurrentMapWidth();
    int y_car_mini_map =
            y_dest_mini_map + (info_my_car.y * mini_map_height) / map_manager.getCurrentMapHeight();

    int size_car_point = 16;
    Area car_dest_area_mini_map(x_car_mini_map - (size_car_point / 2),
                                y_car_mini_map - (size_car_point / 2), size_car_point,
                                size_car_point);

    texture_cars_mini_map.renderEntity(
            Area(0, 0, texture_cars_mini_map.getWidth(), texture_cars_mini_map.getHeight()),
            car_dest_area_mini_map, info_my_car.angle);
}
