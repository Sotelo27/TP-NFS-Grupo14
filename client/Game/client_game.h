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
#include "resources/maps_textures.h"
#include "sdl_wrappers/SdlWindow.h"
#include "sdl_wrappers/SdlAudioManager.h"

#include "info_game.h"
#include "client_helper.h"
#include "game_hud.h"
#include "intermission.h"

class ClientGame: public ConstantRateLoop {
private:
    size_t client_id;
    ServerHandler& server_handler;
    std::vector<PlayerResultTotal>& final_results;
    std::unordered_map<size_t, CarInfoGame> info_players;
    SdlWindow window;
    MapsTextures map_manager;
    IconImprovementManager icon_improvement_manager;
    TimeTickInfo time_info;
    CheatDetector cheat_detector;
    ClientHelper client_helper;
    Intermission intermission_manager;
    AudioManager audio_manager;

    void update_state_from_position();
    void handle_sdl_events();
    void handle_cheat_detection(const char* key_name);
    void handle_movement_input();
    void process_server_messages(ServerMessage::Type expected_type, int msg_limit = -1);

protected:
    void function() final;

public:
    explicit ClientGame(size_t client_id, ServerHandler& server_handler, std::vector<PlayerResultTotal>& final_results);

    void start(MapID selected_map);

    ClientGame(const ClientGame&) = delete;
    ClientGame& operator=(const ClientGame&) = delete;

    ClientGame(ClientGame&&) = default;
    ClientGame& operator=(ClientGame&&) = default;

    ~ClientGame();
};

#endif
