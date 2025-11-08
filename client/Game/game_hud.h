#ifndef GAME_HUD_H
#define GAME_HUD_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "resources/life.h"
#include "resources/maps_textures.h"
#include "sdl_wrappers/SdlWindow.h"
#include "utils/add_text.h"

#include "car_position.h"

class GameHud {
private:
    const SdlWindow& window;
    const MapsTextures& map_manager;
    size_t client_id;
    std::unordered_map<size_t, CarPosition>& car_positions;
    LifeHud life_hud;

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
