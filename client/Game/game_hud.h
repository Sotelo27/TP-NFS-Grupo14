#ifndef GAME_HUD_H
#define GAME_HUD_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "resources/life_hud.h"
#include "resources/maps_textures.h"
#include "resources/time_hud.h"
#include "sdl_wrappers/SdlWindow.h"

#include "car_position.h"

class GameHud {
private:
    const SdlWindow& window;
    const MapsTextures& map_manager;
    size_t client_id;
    std::unordered_map<size_t, CarPosition>& car_positions;
    LifeHud life_hud;
    TimeHud time_hud;
    SdlFont font_hud;

    void renderMiniMap();
    void renderMiniMapBorder(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                             int mini_map_height);
    void renderPositionMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                               int mini_map_height);

    std::string getOrdinalSuffix(int number);
    std::string getOrdinalString(int number);

public:
    explicit GameHud(const SdlWindow& window, const MapsTextures& map_manager, size_t client_id,
                     std::unordered_map<size_t, CarPosition>& car_positions);

    void render();

    GameHud(const GameHud&) = delete;
    GameHud& operator=(const GameHud&) = delete;

    GameHud(GameHud&&) = default;
    GameHud& operator=(GameHud&&) = default;

    ~GameHud() = default;
};

#endif
