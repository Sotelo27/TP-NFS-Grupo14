#ifndef MINI_MAP_HUD_H
#define MINI_MAP_HUD_H

#include <string>
#include <unordered_map>

#include "../car_info_game.h"
#include "../resources/maps_textures.h"
#include "../sdl_wrappers/SdlDrawFill.h"
#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../utils/Area.h"

class MiniMap {
private:
    SdlObjTexture texture_cars_mini_map;
    SdlDrawFill draw_fill;
    size_t client_id;
    const MapsTextures& map_manager;
    std::unordered_map<size_t, CarInfoGame>& info_players;

    void renderMiniMapBorder(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                             int mini_map_height);
    void renderPositionsMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                               int mini_map_height);
    void renderCarOnMiniMap(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width,
                            int mini_map_height, const PlayerTickInfo& info_my_car, bool red);

public:
    explicit MiniMap(size_t client_id, const SdlWindow& window, const MapsTextures& map_manager,
                     std::unordered_map<size_t, CarInfoGame>& info_players);

    void render(int x_dest_mini_map, int y_dest_mini_map, int mini_map_width, int mini_map_height);
};

#endif  // MINI_MAP_HUD_H
