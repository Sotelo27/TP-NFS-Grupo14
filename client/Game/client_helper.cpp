#include "client_helper.h"

#include "constants.h"

#include <unordered_map>
#include <SDL2/SDL.h>

ClientHelper::ClientHelper(size_t client_id, SdlWindow& window,
                           std::unordered_map<size_t, CarInfoGame>& info_players,
                           MapsTextures& map_manager,
                           IconImprovementManager& icon_improvement_manager,
                           TimeTickInfo& time_info):
        client_id(client_id),
        src_area_map(0, 0, 0, 0),
        dest_area_map(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
        info_players(info_players),
        window(window),
        car_sprites(window),
        map_manager(map_manager),
        icon_improvement_manager(icon_improvement_manager),
        game_hud(window, map_manager, client_id, info_players, car_sprites,
                 icon_improvement_manager),
        time_info(time_info) {}

void ClientHelper::update_map_area() {
    const PlayerTickInfo& info_my_car = info_players[client_id].info_car;

    int x_map = info_my_car.x - MAP_WIDTH_SIZE / 2;
    int y_map = info_my_car.y - MAP_HEIGHT_SIZE / 2;

    if (x_map < 0) {
        x_map = 0;
    }

    if (y_map < 0) {
        y_map = 0;
    }

    if (x_map > map_manager.getCurrentMapWidth() - MAP_WIDTH_SIZE) {
        x_map = map_manager.getCurrentMapWidth() - MAP_WIDTH_SIZE;
    }

    if (y_map > map_manager.getCurrentMapHeight() - MAP_HEIGHT_SIZE) {
        y_map = map_manager.getCurrentMapHeight() - MAP_HEIGHT_SIZE;
    }

    src_area_map.update(x_map, y_map, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);
}

void ClientHelper::update_animation_frames() {
    update_map_area();

    Area extend_area_map(src_area_map.getX() - CAR_WIDTH_LARGE,
                         src_area_map.getY() - CAR_HEIGHT_LARGE,
                         src_area_map.getWidth() + CAR_WIDTH_LARGE * 2,
                         src_area_map.getHeight() + CAR_HEIGHT_LARGE * 2);

    for (auto& [id, car]: info_players) {
        if (car.info_car.x < extend_area_map.getX() ||
            car.info_car.x > extend_area_map.getX() + extend_area_map.getWidth() ||
            car.info_car.y < extend_area_map.getY() ||
            car.info_car.y > extend_area_map.getY() + extend_area_map.getHeight()) {
            continue;
        }

        CarData car_data = car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        int x_car_screen = (car.info_car.x - src_area_map.getX()) * MAP_TO_VIEWPORT_SCALE_X;
        int y_car_screen = (car.info_car.y - src_area_map.getY()) * MAP_TO_VIEWPORT_SCALE_Y;
        car.dest_area.update(x_car_screen - car_data.width_scale_screen / 2,
                             y_car_screen - car_data.height_scale_screen / 2,
                             car_data.width_scale_screen, car_data.height_scale_screen);
    }
}

void ClientHelper::render_cars() {
    for (const auto& [id, car]: info_players) {
        if (car.dest_area.getWidth() == 0 || car.dest_area.getHeight() == 0) {
            continue;
        }

        const CarData& car_data =
                car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        car_sprites.render(car_data.area, car.dest_area, car.info_car.angle);
    }
}

void ClientHelper::render_npcs() {
    // Renderiza cada NPC como un círculo rojo 
    SDL_Renderer* renderer = window.getRenderer();
    for (const auto& [id, npc] : npcs_info) {
        // Convertir posición del mapa a pantalla
        int x_screen = (npc.x - src_area_map.getX()) * MAP_TO_VIEWPORT_SCALE_X;
        int y_screen = (npc.y - src_area_map.getY()) * MAP_TO_VIEWPORT_SCALE_Y;
        x_screen += dest_area_map.getX();
        y_screen += dest_area_map.getY();

        // Dibuja un círculo rojo para el NPC
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        int radius = 12;
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx*dx + dy*dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, x_screen + dx, y_screen + dy);
                }
            }
        }
    }
}

void ClientHelper::render_in_z_order(int iteration, bool render_hud) {
    map_manager.render(src_area_map, dest_area_map);

    render_npcs(); // <-- Renderiza NPCs antes o después , lo vemos dsp o como quieras peluche

    render_cars();

    if (render_hud) {
        game_hud.render(iteration, time_info.seconds, src_area_map);
    }

    window.render();
}

void ClientHelper::update_map_info(const std::vector<PlayerTickInfo>& players_info,
                                   const std::vector<NpcTickInfo>& npcs_info_vec,
                                   const TimeTickInfo& time_info) {
    info_players.clear();
    for (const auto& p_info: players_info) {
        info_players[p_info.player_id] = CarInfoGame{p_info, Area()};
    }
    this->time_info = time_info;
    // Actualiza NPCs
    npcs_info.clear();
    for (const auto& npc : npcs_info_vec) {
        npcs_info[npc.npc_id] = npc;
    }
}
