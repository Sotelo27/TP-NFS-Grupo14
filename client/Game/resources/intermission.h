#ifndef INTERMISSION_H
#define INTERMISSION_H

#include <list>
#include <string>

#include "../../../common/constant_rate_loop.h"
#include "../../../common/dto/server_msg.h"
#include "../../connection/server_handler.h"
#include "../sdl_wrappers/SdlFont.h"
#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"

#include "cheat_detector.h"

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
    bool& main_running;
    CheatDetector cheat_detector;
    int iterations;
    SdlObjTexture background_texture;
    SdlFont text_head;
    SdlFont text_position;
    SdlFont text_rest_info;

    void function() final;

    void show_results();

    void handle_sdl_events();
    void handle_cheat_detection(const char* keyName);
    void process_server_messages(ServerMessage::Type expected_type, int msg_limit);

public:
    explicit Intermission(SdlWindow& window, ServerHandler& server_handler, bool& main_running);

    void run();

    ~Intermission() = default;
};

#endif
