#include "mini_map_hud.h"

#include "../../constants.h"

#define CARS_MINI_MAP std::string(ASSETS_PATH) + "/hud/autos.png"

#define WIDTH_CAR_TEXTURE 184
#define HEIGHT_CAR_TEXTURE 112

#define AREA_RED_CAR_MINI_MAP Area(10, 25, WIDTH_CAR_TEXTURE, HEIGHT_CAR_TEXTURE)
#define AREA_BLUE_CAR_MINI_MAP Area(208, 146, WIDTH_CAR_TEXTURE, HEIGHT_CAR_TEXTURE)

MiniMap::MiniMap(size_t client_id, const SdlWindow& window, const MapsTextures& map_manager,
                 std::unordered_map<size_t, CarInfoGame>& info_players):
        texture_cars_mini_map(CARS_MINI_MAP, window,
                              Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
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

    renderPositionsMiniMap(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);
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

void MiniMap::renderCarOnMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                                 int mini_map_height, const PlayerTickInfo& info_car, bool red) {
    int x_car_mini_map =
            x_dest_mini_map + (info_car.x * mini_map_width) / map_manager.getCurrentMapWidth();
    int y_car_mini_map =
            y_dest_mini_map + (info_car.y * mini_map_height) / map_manager.getCurrentMapHeight();

    int size_car_point_width = 25;
    int size_car_point_height = static_cast<float>(size_car_point_width) * HEIGHT_CAR_TEXTURE /
                                WIDTH_CAR_TEXTURE;  // Keep aspect ratio
    Area car_dest_area_mini_map(x_car_mini_map - (size_car_point_width / 2),
                                y_car_mini_map - (size_car_point_height / 2), size_car_point_width,
                                size_car_point_height);

    texture_cars_mini_map.renderEntity(red ? AREA_RED_CAR_MINI_MAP : AREA_BLUE_CAR_MINI_MAP,
                                       car_dest_area_mini_map, info_car.angle);
}

void MiniMap::renderPositionsMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                                     int mini_map_height) {
    for (const auto& [player_id, car_info]: info_players) {
        if (player_id == client_id) {
            continue;
        }

        const PlayerTickInfo& info_car = car_info.info_car;
        renderCarOnMiniMap(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height,
                           info_car, false);
    }

    const PlayerTickInfo& info_my_car = info_players[client_id].info_car;
    renderCarOnMiniMap(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height,
                       info_my_car, true);
}
