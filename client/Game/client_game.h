#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <list>
#include <optional>
#include <string>
#include <unordered_map>

#include "../../common/constant_rate_loop.h"
#include "../connection/server_handler.h"
#include "resources/car_sprite_sheet.h"
#include "resources/cheat_detector.h"
#include "resources/intermission.h"
#include "resources/maps_textures.h"
#include "sdl_wrappers/SdlWindow.h"

#include "car_info_game.h"
#include "game_hud.h"

class ClientGame: public ConstantRateLoop {
private:
    size_t client_id;
    ServerHandler& server_handler;
    bool& game_is_over;
    Area src_area_map;
    Area dest_area_map;
    std::unordered_map<size_t, CarInfoGame> info_players;
    SdlWindow window;
    CarSpriteSheet car_sprites;
    MapsTextures map_manager;
    GameHud game_hud;
    MapID current_map_id;
    TimeTickInfo time_info;
    CheatDetector cheat_detector;
    Intermission intermission_manager;

    uint32_t tiempo_partida_total{0}; // NUEVO: tiempo total de la partida en segundos

    void update_state_from_position();
    void handle_sdl_events();
    void handle_cheat_detection(const char* key_name);
    void handle_movement_input();
    void process_server_messages(ServerMessage::Type expected_type, int msg_limit = -1);

    void update_animation_frames();
    void update_map_area();

    void render_in_z_order();
    void render_cars();

protected:
    void function() final;

public:
    explicit ClientGame(size_t client_id, ServerHandler& server_handler, bool& game_is_over);

    void start();

    ClientGame(const ClientGame&) = delete;
    ClientGame& operator=(const ClientGame&) = delete;

    ClientGame(ClientGame&&) = default;
    ClientGame& operator=(ClientGame&&) = default;

    ~ClientGame();
};

#endif
