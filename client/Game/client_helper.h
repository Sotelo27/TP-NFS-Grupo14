#ifndef CLIENT_HELPER_H
#define CLIENT_HELPER_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "resources/car_sprite_sheet.h"
#include "resources/cheat_detector.h"
#include "resources/maps_textures.h"
#include "sdl_wrappers/SdlWindow.h"

#include "info_game.h"
#include "game_hud.h"

class ClientHelper {
private:
    size_t client_id;
    Area src_area_map;
    Area dest_area_map;
    std::unordered_map<size_t, CarInfoGame>& info_players;
    std::vector<NpcInfoGame> npcs_info;
    SdlWindow& window;
    CarSpriteSheet car_sprites;
    MapsTextures& map_manager;
    IconImprovementManager& icon_improvement_manager;
    GameHud game_hud;
    TimeTickInfo& time_info;

    void update_map_area();

    void render_cars();
    void render_npcs();

public:
    explicit ClientHelper(size_t client_id, SdlWindow& window,
                          std::unordered_map<size_t, CarInfoGame>& info_players,
                          MapsTextures& map_manager,
                          IconImprovementManager& icon_improvement_manager,
                          TimeTickInfo& time_info);

    void update_animation_frames();
    void render_in_z_order(int iteration, bool render_hud = true);
    void update_map_info(const std::vector<PlayerTickInfo>& players_info,
                         const std::vector<NpcTickInfo>& npcs_info,
                         const TimeTickInfo& time_info);

    ClientHelper(const ClientHelper&) = delete;
    ClientHelper& operator=(const ClientHelper&) = delete;

    ClientHelper(ClientHelper&&) = default;
    ClientHelper& operator=(ClientHelper&&) = default;

    ~ClientHelper() = default;
};

#endif
