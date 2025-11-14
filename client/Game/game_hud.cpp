#include "game_hud.h"

#include "sdl_wrappers/SdlDrawFill.h"
#include "sdl_wrappers/SdlObjTexture.h"

#include "constants.h"

#define POINT_RED std::string(ASSETS_PATH) + "/images/punto_rojo.png"

#define SPACE_BETWEEN_WINDOW_EDGE_AND_HUD 15

GameHud::GameHud(const SdlWindow& window, const MapsTextures& map_manager, size_t client_id,
                 std::unordered_map<size_t, CarInfoGame>& info_players,
                 const CarSpriteSheet& car_sprites):
        window(window),
        map_manager(map_manager),
        client_id(client_id),
        info_players(info_players),
        life_hud(window),
        time_hud(window),
        life_bar_sprites(window),
        car_sprites(car_sprites),
        speed_hud(window),
        position_hud(window),
        mini_map(client_id, window, map_manager, info_players),
        hint(window) {}

void GameHud::renderMiniMap() {
    int y_dest_mini_map = SPACE_BETWEEN_WINDOW_EDGE_AND_HUD;
    int x_dest_mini_map = WINDOW_WIDTH - (MAP_HEIGHT_SIZE * 3 / 4) - y_dest_mini_map;
    int mini_map_width = 300;
    int mini_map_height =
            mini_map_width * map_manager.getCurrentMapHeight() / map_manager.getCurrentMapWidth();

    mini_map.render(x_dest_mini_map, y_dest_mini_map, mini_map_width, mini_map_height);
}

void GameHud::renderLifeBarHud() {
    for (const auto& [id, car]: info_players) {
        if (car.dest_area.getWidth() == 0 || car.dest_area.getHeight() == 0 ||
            car.info_car.player_id == client_id) {
            continue;
        }

        const CarData& car_data =
                car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        // falta la vida maxima, que se espera recibir al principio de la partida
        life_bar_sprites.render(
                100, car.info_car.health,
                Area(car.dest_area.getX(), car.dest_area.getY() - car_data.width_scale_screen / 5,
                     car_data.width_scale_screen, car_data.width_scale_screen / 5));
    }
}

void GameHud::render(int iteration) {
    renderLifeBarHud();

    const CarData& client_car_data = car_sprites.getCarData(
            static_cast<CarSpriteID>(info_players[client_id].info_car.car_id));
    hint.render(info_players[client_id].dest_area.getX(), info_players[client_id].dest_area.getY(),
                100, 0.0, iteration, client_car_data.width_scale_screen,
                client_car_data.height_scale_screen);

    speed_hud.render(999, WINDOW_WIDTH - WINDOW_WIDTH / 7, WINDOW_HEIGHT - 210);

    position_hud.render(3, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD);

    time_hud.render(600, WINDOW_WIDTH / 2, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD);

    int current_life = info_players[client_id].info_car.health;
    // falta obtener la vida maxima
    life_hud.render(100, current_life, 20, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD + 60);

    renderMiniMap();
}
