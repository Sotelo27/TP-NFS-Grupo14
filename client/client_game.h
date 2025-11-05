#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "connection/server_handler.h"
#include "sdl_wrappers/SdlWindow.h"
#include "utils/add_text.h"
#include "utils/car_sprite_sheet.h"
#include "utils/maps_textures.h"

struct Position {
    int x_car_map;
    int y_car_map;
    float angle;
};

struct CarPosition {
    Position position;
    Area dest_area;
};

class ClientGame {
private:
    size_t client_id;
    ServerHandler& server_handler;
    bool running;
    Area src_area_map;
    Area dest_area_map;
    std::unordered_map<size_t, CarPosition> car_positions;
    CarSpriteID current_car = CarSpriteID::CommonGreenCar;

    void update_state_from_position();
    void update_animation_frames(const MapData& map_data, const CarSpriteSheet& car_sprites);
    void update_map_area(const MapData& map_data);

    void render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                           const CarSpriteSheet& car_sprites, const AddText& add_text);

public:
    explicit ClientGame(size_t client_id, ServerHandler& server_handler);

    void start();

    ClientGame(const ClientGame&) = delete;
    ClientGame& operator=(const ClientGame&) = delete;

    ClientGame(ClientGame&&) = default;
    ClientGame& operator=(ClientGame&&) = default;

    ~ClientGame();
};

#endif
