#ifndef INTERMISSION_H
#define INTERMISSION_H

#include <list>
#include <string>
#include <vector>

#include "../../../common/constant_rate_loop.h"
#include "../../../common/dto/server_msg.h"
#include "../../connection/server_handler.h"
#include "../sdl_wrappers/SdlFont.h"
#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"

#include "cheat_detector.h"
#include "maps_textures.h"

struct PlayerInfoI {
    int position;
    std::string name;
    int race_time_seconds;
    int total_time_seconds;
};

class Intermission: public ConstantRateLoop {
private:
    SdlWindow& window;
    ServerHandler& server_handler;
    MapsTextures& map_manager;
    bool& main_running;
    CheatDetector cheat_detector;
    SdlObjTexture background_info;
    SdlObjTexture background_improvement;
    SdlObjTexture next_button;
    SdlFont text_head;
    SdlFont text_position;
    SdlFont text_rest_info;
    bool improvement_phase;
    int iteration_init_improvement_phase;
    std::vector<PlayerInfoI> player_infos;

    void function() final;

    void show_results();
    void show_table_results(const std::vector<PlayerInfoI>& player_infos);
    void show_info_center(SdlFont& font, const std::string& info, int x_start, int x_end,
                          int y_info, const Rgb& color_front, const Rgb& color_shadow);
    void show_text_for_next_phase();

    void show_improvement_phase();

    void handle_sdl_events();
    void handle_cheat_detection(const char* keyName);
    void handle_key_pressed(const char* keyName);
    void process_server_messages(ServerMessage::Type expected_type, int msg_limit);

public:
    explicit Intermission(SdlWindow& window, ServerHandler& server_handler,
                          MapsTextures& map_manager, bool& main_running);

    void run(std::vector<PlayerInfoI> player_infos);

    ~Intermission() = default;
};

#endif
