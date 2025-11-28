#ifndef INTERMISSION_H
#define INTERMISSION_H

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "../../../common/constant_rate_loop.h"
#include "../../../common/dto/results_info.h"
#include "../../../common/dto/server_msg.h"
#include "../../../common/enum/car_improvement.h"
#include "../../connection/server_handler.h"
#include "../sdl_wrappers/SdlFont.h"
#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"

#include "cheat_detector.h"
#include "maps_textures.h"

struct ImprovementOption {
    CarImprovement improvement_id;
    std::string key;
    SdlObjTexture& icon;
    std::string improvement;
    std::string description;
    Rgb color;
};

struct RenderContext {
    int iteration_phase;
    int y_offset;
    int time_balance;
    int y_start_options;
    int y_limit_options;
    int option_height;
};

struct DataImprovementOption {
    bool is_selected;
    SdlObjTexture& icon;
};

struct EventPurchaseImprovement {
    int cost;
    SdlObjTexture& icon;
    int iteration_purchase;
};

class Intermission: public ConstantRateLoop {
private:
    size_t client_id;
    SdlWindow& window;
    ServerHandler& server_handler;
    MapsTextures& map_manager;
    bool& main_running;
    CheatDetector cheat_detector;
    SdlObjTexture background_info;
    SdlObjTexture background_improvement;
    SdlObjTexture button_upgrade;
    SdlObjTexture icon_controllability;
    SdlFont text_head;
    SdlFont text_position;
    SdlFont text_rest_info;
    SdlFont text_keys;
    bool improvement_phase;
    int iteration_init_improvement_phase;
    std::vector<PlayerResultCurrent> player_infos;
    std::vector<ImprovementOption> improvement_options;
    std::unordered_map<CarImprovement, DataImprovementOption> selected_improvements;
    std::list<EventPurchaseImprovement> improvements_purchased;

    void function() final;

    void show_results();
    void show_table_results();
    void show_info_center(SdlFont& font, const std::string& info, int x_start, int x_end,
                          int y_info, const Rgb& color_front, const Rgb& color_shadow);
    void show_text_for_next_phase();

    void show_improvement_phase();
    bool render_background(const RenderContext& ctx);
    bool render_clock(const RenderContext& ctx);
    bool render_title(RenderContext& ctx);
    bool render_time_balance(RenderContext& ctx);
    void render_improvements_purchase(const RenderContext& ctx);
    bool render_improvement_options(RenderContext& ctx);

    void calculate_layout(RenderContext& ctx);
    void render_single_option(const ImprovementOption& option, int index, const RenderContext& ctx);

    void handle_sdl_events();
    void handle_cheat_detection(const char* keyName);
    void handle_key_pressed(const char* keyName);
    void process_server_messages(ServerMessage::Type expected_type, int msg_limit = -1);

public:
    explicit Intermission(size_t client_id, SdlWindow& window, ServerHandler& server_handler,
                          MapsTextures& map_manager, bool& main_running);

    void run(std::vector<PlayerResultCurrent> player_infos);

    ~Intermission() = default;
};

#endif
