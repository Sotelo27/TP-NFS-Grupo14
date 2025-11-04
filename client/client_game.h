#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "connection/server_handler.h"
#include "sdl_wrappers/SdlWindow.h"
#include "utils/car_sprite_sheet.h"
#include "utils/maps_textures.h"

struct Positions {
    int x_car_map;
    int y_car_map;
};

class ClientGame {
private:
    CarSpriteID current_car;
    size_t client_id;
    ServerHandler& server_handler;
    bool running;
    Positions positions;
    Area src_area_map;
    Area dest_area_map;
    std::unordered_map<size_t, Area> map_dest_areas;

    void update_state_from_position();
    void update_animation_frames(const MapData& map_data, const CarSpriteSheet& car_sprites);
    void render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                           const CarSpriteSheet& car_sprites);

public:
    explicit ClientGame(CarSpriteID current_car, size_t client_id,
        ServerHandler& server_handler);

    void start();

    ClientGame(const ClientGame&) = delete;
    ClientGame& operator=(const ClientGame&) = delete;

    ClientGame(ClientGame&&) = default;
    ClientGame& operator=(ClientGame&&) = default;

    ~ClientGame();
};

#endif
