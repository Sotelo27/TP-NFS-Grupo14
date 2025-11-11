#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "../connection/server_handler.h"
#include "resources/car_sprite_sheet.h"
#include "resources/maps_textures.h"
#include "sdl_wrappers/SdlWindow.h"

#include "car_info_game.h"
#include "game_hud.h"

class ClientGame {
private:
    size_t client_id;
    ServerHandler& server_handler;
    bool running;
    Area src_area_map;
    Area dest_area_map;
    std::unordered_map<size_t, CarInfoGame> info_players;
    CarSpriteID current_car = CarSpriteID::CommonGreenCar;

    void update_state_from_position();

    void update_animation_frames(const MapsTextures& map_manager,
                                 const CarSpriteSheet& car_sprites);
    void update_map_area(const MapsTextures& map_manager);

    void render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                           const CarSpriteSheet& car_sprites, GameHud& game_hud);
    void render_cars(const CarSpriteSheet& car_sprites);

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
