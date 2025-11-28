#include "game_hud.h"

#include "sdl_wrappers/SdlDrawFill.h"
#include "sdl_wrappers/SdlObjTexture.h"

#include "constants.h"

#define POINT_RED std::string(ASSETS_PATH) + "/images/punto_rojo.png"

#define SPACE_BETWEEN_WINDOW_EDGE_AND_HUD 15

GameHud::GameHud(const SdlWindow& window, const MapsTextures& map_manager, size_t client_id,
                 std::unordered_map<size_t, CarInfoGame>& info_players,
                 const CarSpriteSheet& car_sprites, IconImprovementManager& icon_manager):
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
        hint(window),
        checkpoint(window),
        icon_improvement_manager(icon_manager) {}

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

void GameHud::renderHint(const CarInfoGame& client_car, int iteration) {
    if (client_car.info_car.distance_to_checkpoint < 265) {
        return;
    }

    const CarData& client_car_data =
            car_sprites.getCarData(static_cast<CarSpriteID>(client_car.info_car.car_id));
    hint.render(client_car.dest_area.getX(), client_car.dest_area.getY(),
                client_car.info_car.distance_to_checkpoint, client_car.info_car.hint_angle_deg,
                iteration, client_car_data.width_scale_screen, client_car_data.height_scale_screen);
}

void GameHud::renderPurchasedImprovement() {
    std::list<SdlObjTexture*> improvements;
    improvements.push_back(&icon_improvement_manager.get_icon(CarImprovement::Controllability));
    improvements.push_back(&icon_improvement_manager.get_icon(CarImprovement::Health));
    improvements.push_back(&icon_improvement_manager.get_icon(CarImprovement::Speed));
    improvements.push_back(&icon_improvement_manager.get_icon(CarImprovement::Acceleration));
    improvements.push_back(&icon_improvement_manager.get_icon(CarImprovement::Mass));

    int x_start = SPACE_BETWEEN_WINDOW_EDGE_AND_HUD;
    int y_start = WINDOW_HEIGHT - 75;
    for (SdlObjTexture* improvement_icon: improvements) {
        int size_height = 55;
        int size_width =
                (improvement_icon->getWidth() * size_height) / improvement_icon->getHeight();
        improvement_icon->render(
                Area(0, 0, improvement_icon->getWidth(), improvement_icon->getHeight()),
                Area(x_start, y_start, size_width, size_height));

        x_start += size_width + 8;
    }
}

void GameHud::render(int iteration, int time_seconds, const Area& src_area_map) {
    const CarInfoGame& client_car = info_players[client_id];

    checkpoint.render(client_car.info_car.x_checkpoint, client_car.info_car.y_checkpoint,
                      src_area_map);

    renderLifeBarHud();

    renderHint(client_car, iteration);

    speed_hud.render(client_car.info_car.speed_mps, WINDOW_WIDTH - WINDOW_WIDTH / 7,
                     WINDOW_HEIGHT - WINDOW_HEIGHT / 7);

    position_hud.render(3, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD);

    time_hud.render(time_seconds, WINDOW_WIDTH / 2, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD);

    int current_life = client_car.info_car.health;
    // falta obtener la vida maxima
    life_hud.render(100, current_life, 20, SPACE_BETWEEN_WINDOW_EDGE_AND_HUD + 60);

    renderMiniMap();

    renderPurchasedImprovement();
}
