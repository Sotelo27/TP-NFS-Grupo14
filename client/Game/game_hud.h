#ifndef GAME_HUD_H
#define GAME_HUD_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "resources/car_sprite_sheet.h"
#include "resources/hud/checkpoint.h"
#include "resources/hud/hint.h"
#include "resources/hud/icon_improvement_manager.h"
#include "resources/hud/life_bar.h"
#include "resources/hud/life_hud.h"
#include "resources/hud/mini_map_hud.h"
#include "resources/hud/position_hud.h"
#include "resources/hud/speed_hud.h"
#include "resources/hud/time_hud.h"
#include "resources/maps_textures.h"
#include "sdl_wrappers/SdlWindow.h"

#include "info_game.h"

class GameHud {
private:
    const SdlWindow& window;
    const MapsTextures& map_manager;
    size_t client_id;
    std::unordered_map<size_t, CarInfoGame>& info_players;
    LifeHud life_hud;
    TimeHud time_hud;
    LifeBarSpriteSheet life_bar_sprites;
    const CarSpriteSheet& car_sprites;
    SpeedHud speed_hud;
    PositionHud position_hud;
    MiniMap mini_map;
    Hint hint;
    Checkpoint checkpoint;
    IconImprovementManager& icon_improvement_manager;

    void renderLifeBarHud();
    void renderMiniMap();
    void renderHint(const CarInfoGame& client_car, int iteration);
    void renderPurchasedImprovement();

public:
    explicit GameHud(const SdlWindow& window, const MapsTextures& map_manager, size_t client_id,
                     std::unordered_map<size_t, CarInfoGame>& info_players,
                     const CarSpriteSheet& car_sprites,
                     IconImprovementManager& icon_improvement_manager);

    void render(int iteration, int time_seconds, const Area& src_area_map);

    GameHud(const GameHud&) = delete;
    GameHud& operator=(const GameHud&) = delete;

    GameHud(GameHud&&) = default;
    GameHud& operator=(GameHud&&) = default;

    ~GameHud() = default;
};

#endif
