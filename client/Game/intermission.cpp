#include "intermission.h"

#include <algorithm>
#include <string>
#include <utility>

#include "utils/time_formatter.h"

#include "constants.h"

#define BACKGROUND_INFO_IMAGE_PATH std::string(ASSETS_PATH) + "/images/fondo_cars.jpg"
#define BACKGROUND_IMPROVEMENT_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/garaje.png"
#define BUTTON_UPGRADE_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/boton_u.png"

#define SIZE_TEXT_HEAD (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5
#define SIZE_TEXT_POSITION (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5
#define SIZE_TEXT_REST_INFO (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5

#define SIZE_ICON_BUTTON (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5

#define X_LIMIT_OPTION (static_cast<float>(SIZE_TEXT_HEAD) * 7 / 4)

#define WHITE Rgb(255, 255, 255)

#define DEEP_INDIGO_VIOLET Rgb(30, 0, 90)
#define DARK_PURPLE_VIOLET Rgb(60, 0, 60)
#define DARK_VIOLET Rgb(45, 0, 75)
#define VIBRANT_VIOLET Rgb(140, 0, 150)
#define NEON_MAGENTA Rgb(215, 40, 185)
#define SOFT_NEON_PINK Rgb(255, 110, 180)

#define GLITCH_VIOLET Rgb(100, 0, 255)
#define PURPLE_NEON_BRIGHT Rgb(180, 0, 255)
#define MAGENTA_VIBRANT Rgb(255, 0, 200)
#define ELECTRIC_PINK Rgb(255, 100, 255)

#define ORANGE_SUNSET Rgb(255, 125, 0)
#define BRIGHT_FIRE_YELLOW Rgb(255, 255, 100)
#define ORANGE_SUN Rgb(255, 140, 0)
#define GOLDEN_YELLOW Rgb(255, 180, 0)
#define NEON_YELLOW Rgb(255, 255, 0)
#define NEON_LIME Rgb(150, 255, 0)
#define ELECTRIC_MINT_GREEN Rgb(50, 255, 150)
#define ELECTRIC_CYAN Rgb(0, 255, 255)
#define NEON_WATER_BLUE Rgb(0, 180, 255)

#define NEON_RED Rgb(255, 0, 0)

#define VIBRANT_ORANGE Rgb(255, 100, 0)
#define BLUE Rgb(0, 0, 255)
#define RED Rgb(255, 0, 0)
#define GLITCH_LIGHT_BLUE Rgb(50, 150, 255)

#define NEON_YO Rgb(255, 200, 0)

constexpr int AMOUNT_FRAMES_ANIMATION = 90;
constexpr int AMOUNT_FRAMES_WAITING = 20;
constexpr int RESULTS = AMOUNT_FRAMES_ANIMATION + 2 * AMOUNT_FRAMES_WAITING;

constexpr int AMOUNT_FRAMES_TO_RENDER_CLOCK = AMOUNT_FRAMES_ANIMATION + AMOUNT_FRAMES_WAITING;
constexpr int AMOUNT_FRAMES_TO_RENDER_TITLE = AMOUNT_FRAMES_TO_RENDER_CLOCK + AMOUNT_FRAMES_WAITING;
constexpr int AMOUNT_FRAMES_TO_RENDER_TIME_BALANCE =
        AMOUNT_FRAMES_TO_RENDER_TITLE + AMOUNT_FRAMES_WAITING;
constexpr int AMOUNT_FRAMES_CLOSE_TO_END =
        AMOUNT_FRAMES_TO_RENDER_TIME_BALANCE + AMOUNT_FRAMES_WAITING - 1;

#define KEY_NEXT_BUTTON "N"
const char NEXT_PHASE_TEXT[] = "Press " KEY_NEXT_BUTTON " to continue...";

const char KEY_IMPROVEMENT_SPEED = 'S';
const char KEY_IMPROVEMENT_HEALTH = 'H';
const char KEY_IMPROVEMENT_ACCELERATION = 'A';
const char KEY_IMPROVEMENT_MASS = 'M';
const char KEY_IMPROVEMENT_CONTROLLABILITY = 'C';

Intermission::Intermission(size_t client_id, SdlWindow& window, ServerHandler& server_handler,
                           MapsTextures& map_manager, bool& main_running,
                           IconImprovementManager& icon_manager, ClientHelper& client_helper):
        ConstantRateLoop(FRAME_RATE),
        client_id(client_id),
        window(window),
        server_handler(server_handler),
        map_manager(map_manager),
        main_running(main_running),
        cheat_detector(5),
        background_info(BACKGROUND_INFO_IMAGE_PATH, window, Rgb(0, 255, 0)),
        background_improvement(BACKGROUND_IMPROVEMENT_IMAGE_PATH, window, Rgb(0, 255, 0)),
        button_upgrade(BUTTON_UPGRADE_IMAGE_PATH, window, RGB_BACKGROUND),
        icon_manager(icon_manager),
        text_head(FONT_STYLE_PX, SIZE_TEXT_HEAD, window),
        text_position(FONT_STYLE_VS1, SIZE_TEXT_POSITION, window),
        text_rest_info(FONT_STYLE_CC, SIZE_TEXT_REST_INFO, window),
        text_keys(FONT_STYLE_AA, SIZE_TEXT_REST_INFO, window),
        improvement_phase(false),
        iteration_init_improvement_phase(0),
        player_infos(),
        improvement_options(),
        selected_improvements(),
        client_helper(client_helper),
        iteration_called(0),
        iteration_breakpoint(0),
        ready_next_race(false),
        current_balance(0) {
    initialize_improvement_options();

    initialize_selected_improvements();
}

void Intermission::initialize_improvement_options() {
    improvement_options.push_back({CarImprovement::Health, std::string(1, KEY_IMPROVEMENT_HEALTH),
                                   icon_manager.get_icon(CarImprovement::Health), "Health",
                                   "Survive more hits", NEON_YELLOW});
    improvement_options.push_back({CarImprovement::Speed, std::string(1, KEY_IMPROVEMENT_SPEED),
                                   icon_manager.get_icon(CarImprovement::Speed), "Speed",
                                   "Higher maximum speed", NEON_LIME});
    improvement_options.push_back({CarImprovement::Controllability,
                                   std::string(1, KEY_IMPROVEMENT_CONTROLLABILITY),
                                   icon_manager.get_icon(CarImprovement::Controllability),
                                   "Controllability", "Better turning", ELECTRIC_MINT_GREEN});
    improvement_options.push_back({CarImprovement::Acceleration,
                                   std::string(1, KEY_IMPROVEMENT_ACCELERATION),
                                   icon_manager.get_icon(CarImprovement::Acceleration),
                                   "Acceleration", "Quicker 0-100 km/h", ELECTRIC_CYAN});
    improvement_options.push_back({CarImprovement::Mass, std::string(1, KEY_IMPROVEMENT_MASS),
                                   icon_manager.get_icon(CarImprovement::Mass), "Mass",
                                   "Stronger collisions", NEON_WATER_BLUE});
}

void Intermission::initialize_selected_improvements() {
    selected_improvements.emplace(
            CarImprovement::Health,
            DataImprovementOption{false, icon_manager.get_icon(CarImprovement::Health)});
    selected_improvements.emplace(
            CarImprovement::Speed,
            DataImprovementOption{false, icon_manager.get_icon(CarImprovement::Speed)});
    selected_improvements.emplace(
            CarImprovement::Acceleration,
            DataImprovementOption{false, icon_manager.get_icon(CarImprovement::Acceleration)});
    selected_improvements.emplace(
            CarImprovement::Mass,
            DataImprovementOption{false, icon_manager.get_icon(CarImprovement::Mass)});
    selected_improvements.emplace(
            CarImprovement::Controllability,
            DataImprovementOption{false, icon_manager.get_icon(CarImprovement::Controllability)});
}

void Intermission::function() {
    handle_sdl_events();

    process_server_messages(ServerMessage::Type::Empty, 10);

    window.fill();

    show_background_game();

    if (!ready_next_race) {
        show_results();
    } else {
        this->running = iteration_breakpoint > iteration ? true : false;
    }

    if (improvement_phase) {
        show_improvement_phase();
    }

    window.render();
}

void Intermission::show_background_game() {
    bool should_render = false;

    if (iteration <= AMOUNT_FRAMES_ANIMATION) {
        should_render = true;
    }

    if (ready_next_race) {
        client_helper.update_animation_frames();
        should_render = true;
    }

    if (should_render) {
        client_helper.render_in_z_order(iteration_called);
    }
}

void Intermission::run(std::vector<PlayerResultCurrent> player_infos, int iteration_called) {
    std::sort(player_infos.begin(), player_infos.end(),
              [](const PlayerResultCurrent& a, const PlayerResultCurrent& b) {
                  return a.position < b.position;
              });
    this->player_infos = std::move(player_infos);

    this->iteration_called = iteration_called;

    clear_resources();

    ConstantRateLoop::start_loop();
}

void Intermission::clear_resources() {
    for (auto& pair: selected_improvements) {
        pair.second.is_selected = false;
    }
    improvements_purchased.clear();

    improvement_phase = false;

    iteration_breakpoint = 0;

    ready_next_race = false;
}

void Intermission::show_info_center(SdlFont& font, const std::string& info, int x_start, int x_end,
                                    int y_info, const Rgb& color_front, const Rgb& color_shadow) {
    font.loadText(info, WHITE, true);
    int x_position_center = x_start + (x_end - x_start) / 2 - font.getWidth() / 2;
    font.renderDirect(x_position_center, y_info, info, color_front, true, color_shadow);
}

void Intermission::show_table_results() {
    int y_head = SIZE_TEXT_HEAD;
    int x_postion_start = SIZE_TEXT_HEAD;
    text_head.renderDirect(x_postion_start, y_head, "POSITION", GLITCH_VIOLET, true, WHITE);
    int x_position_end = text_head.getWidth() + x_postion_start;

    int x_player_start = x_position_end + SIZE_TEXT_HEAD * 3;
    text_head.renderDirect(x_player_start, y_head, "PLAYER", PURPLE_NEON_BRIGHT, true, WHITE);
    int x_player_end = x_player_start + text_head.getWidth();

    int x_race_time_start = x_player_end + SIZE_TEXT_HEAD * 3;
    text_head.renderDirect(x_race_time_start, y_head, "RACE TIME", MAGENTA_VIBRANT, true, WHITE);
    int x_race_time_end = x_race_time_start + text_head.getWidth();

    int x_total_time_start = x_race_time_end + SIZE_TEXT_HEAD;
    text_head.renderDirect(x_total_time_start, y_head, "TOTAL TIME", ELECTRIC_PINK, true, WHITE);
    int x_total_time_end = x_total_time_start + text_head.getWidth();

    if (iteration <= RESULTS) {
        return;
    }

    int frames = iteration - RESULTS;
    int min = std::min(static_cast<int>(player_infos.size()), 8);
    int n = std::min((frames / AMOUNT_FRAMES_WAITING), min);
    int y_limit = y_head + static_cast<float>(SIZE_TEXT_HEAD) / 2 + text_head.getHeight();
    for (int i = 0; i < n; i++) {
        const PlayerResultCurrent& player_info = player_infos[i];
        int y_info = y_limit + (SIZE_TEXT_REST_INFO + SIZE_TEXT_HEAD / 8) * i;

        show_info_center(text_position, std::to_string(player_info.position), x_postion_start,
                         x_position_end, y_info, WHITE, GLITCH_VIOLET);

        show_info_center(text_rest_info, player_info.username, x_player_start, x_player_end, y_info,
                         WHITE, PURPLE_NEON_BRIGHT);

        show_info_center(text_rest_info, std::to_string(player_info.race_time_seconds) + "s",
                         x_race_time_start, x_race_time_end, y_info, WHITE, MAGENTA_VIBRANT);

        show_info_center(text_rest_info, std::to_string(player_info.total_time_seconds) + "s",
                         x_total_time_start, x_total_time_end, y_info, WHITE, ELECTRIC_PINK);
    }
}

void Intermission::show_text_for_next_phase() {
    text_head.loadText(NEXT_PHASE_TEXT, WHITE, true);
    int x_position = WINDOW_WIDTH - SIZE_TEXT_HEAD - text_head.getWidth();
    int y_position = WINDOW_HEIGHT - SIZE_TEXT_HEAD - text_head.getHeight() / 2;
    text_head.renderDirect(x_position, y_position, NEXT_PHASE_TEXT, ORANGE_SUN, true, WHITE);
}

void Intermission::show_results() {
    // falta atajar que el string no se pase del ancho de la pantalla
    int background_frame_limit = std::min(iteration, AMOUNT_FRAMES_ANIMATION);
    int y_animation =
            (background_info.getHeight() * background_frame_limit) / AMOUNT_FRAMES_ANIMATION;
    int y_window = (WINDOW_HEIGHT * background_frame_limit) / AMOUNT_FRAMES_ANIMATION;
    background_info.renderEntity(
            Area(0, y_animation - 1, background_info.getWidth(), background_info.getHeight()),
            Area(0, 0, WINDOW_WIDTH, y_window), 0.0);

    if (iteration > AMOUNT_FRAMES_ANIMATION + AMOUNT_FRAMES_WAITING) {
        show_table_results();
    }

    float frames = iteration - RESULTS;
    if ((frames / AMOUNT_FRAMES_WAITING) >= player_infos.size() + 1) {
        show_text_for_next_phase();
    }
}

void Intermission::process_server_messages(ServerMessage::Type expected_type, int msg_limit) {
    int msg_count = 0;
    bool keep_loop = true;

    while (keep_loop && (msg_limit == -1 || msg_count < msg_limit)) {
        ServerMessage action = server_handler.recv_response_from_server();

        if (action.type == ServerMessage::Type::RaceStart) {
            // map_manager.loadMap(static_cast<MapID>(action.map_id));
            keep_loop = false;
            iteration_breakpoint = iteration + AMOUNT_FRAMES_CLOSE_TO_END;
            ready_next_race = true;
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
            this->running = false;
            std::cout << "[ClientGame] Received Unknown message from server, probably "
                         "disconnected. Exiting..."
                      << std::endl;
        } else if (action.type == ServerMessage::Type::ImprovementOK) {
            const ImprovementResult& r = action.result_market_player;
            if (static_cast<CarImprovement>(r.improvement_id) == CarImprovement::Init) {
                current_balance = r.current_balance;
            } else if (r.player_id == client_id && r.ok) {
                auto it = selected_improvements.find(static_cast<CarImprovement>(r.improvement_id));
                if (it != selected_improvements.end()) {
                    it->second.is_selected = true;
                    improvements_purchased.push_back({static_cast<int>(r.total_penalty_seconds),
                                                      it->second.icon, iteration});
                }
                current_balance = r.current_balance;
            }
        } else if (action.type == ServerMessage::Type::MarketTime) {
            time_market = action.race_time.seconds;
        } else if (action.type == ServerMessage::Type::MapInfo && ready_next_race) {
            client_helper.update_map_info(action.players_tick, action.race_time);
        }

        if (action.type == expected_type) {
            keep_loop = false;
        }

        msg_count++;
    }
}

void Intermission::handle_cheat_detection(const char* keyName) {
    cheat_detector.add_key(keyName);

    if (cheat_detector.check_cheat("Q")) {
        std::cout << "[ClientGame] Cheat code EXIT detected. Exiting game." << std::endl;
        running = false;
        main_running = false;
    } else if (cheat_detector.check_cheat("MID")) {
        running = false;
    } else if (cheat_detector.check_cheat("NEXT")) {
        iteration_breakpoint = iteration + AMOUNT_FRAMES_CLOSE_TO_END;
        ready_next_race = true;
    }
}

void Intermission::handle_key_pressed(const char* keyName) {
    if (keyName == std::string(KEY_NEXT_BUTTON)) {
        iteration_init_improvement_phase =
                improvement_phase ? iteration_init_improvement_phase : iteration;
        improvement_phase = true;
    }

    if (improvement_phase) {
        if (keyName == std::string(1, KEY_IMPROVEMENT_SPEED)) {
            server_handler.send_improvement_choice(CarImprovement::Speed);
        } else if (keyName == std::string(1, KEY_IMPROVEMENT_HEALTH)) {
            server_handler.send_improvement_choice(CarImprovement::Health);
        } else if (keyName == std::string(1, KEY_IMPROVEMENT_ACCELERATION)) {
            server_handler.send_improvement_choice(CarImprovement::Acceleration);
        } else if (keyName == std::string(1, KEY_IMPROVEMENT_MASS)) {
            server_handler.send_improvement_choice(CarImprovement::Mass);
        } else if (keyName == std::string(1, KEY_IMPROVEMENT_CONTROLLABILITY)) {
            server_handler.send_improvement_choice(CarImprovement::Controllability);
        }
    }
}

void Intermission::handle_sdl_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;

                const char* keyName = SDL_GetKeyName(keyEvent.keysym.sym);

                handle_key_pressed(keyName);

                handle_cheat_detection(keyName);
            } break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_QUIT:
                std::cout << "[ClientGame] Quit event received" << std::endl;
                running = false;
                main_running = false;
                break;
        }
    }
}

void Intermission::show_improvement_phase() {
    RenderContext ctx;
    ctx.iteration_phase = iteration - iteration_init_improvement_phase;
    if (ready_next_race) {
        ctx.iteration_phase = iteration_breakpoint - iteration;
    }

    if (!render_background(ctx))
        return;
    if (!render_clock(ctx))
        return;
    if (!render_title(ctx))
        return;
    render_time_balance(ctx);
    render_improvement_options(ctx);
}

bool Intermission::render_background(const RenderContext& ctx) {
    int background_frame_limit = std::min(ctx.iteration_phase, AMOUNT_FRAMES_ANIMATION);
    int y_animation =
            (background_improvement.getHeight() * background_frame_limit) / AMOUNT_FRAMES_ANIMATION;
    int y_window = (WINDOW_HEIGHT * background_frame_limit) / AMOUNT_FRAMES_ANIMATION;
    background_improvement.renderEntity(
            Area(0, background_improvement.getHeight() - y_animation,
                 background_improvement.getWidth(), background_improvement.getHeight()),
            Area(0, 0, WINDOW_WIDTH, y_window), 0.0);

    return ctx.iteration_phase >= AMOUNT_FRAMES_TO_RENDER_CLOCK;
}

bool Intermission::render_clock(const RenderContext& ctx) {
    std::string time_str = TimeFormatter::format_time(time_market);
    show_info_center(text_head, time_str, WINDOW_WIDTH - SIZE_TEXT_HEAD * 3,
                     WINDOW_WIDTH - SIZE_TEXT_HEAD * 2, SIZE_TEXT_HEAD / 2, BRIGHT_FIRE_YELLOW,
                     DARK_VIOLET);

    return ctx.iteration_phase >= AMOUNT_FRAMES_TO_RENDER_TITLE;
}

bool Intermission::render_title(RenderContext& ctx) {
    ctx.y_offset = SIZE_TEXT_HEAD + SIZE_TEXT_HEAD / 4;
    show_info_center(text_head, "CAR UPGRADE", SIZE_TEXT_HEAD, WINDOW_WIDTH - SIZE_TEXT_HEAD,
                     ctx.y_offset, ORANGE_SUN, DARK_VIOLET);

    return ctx.iteration_phase >= AMOUNT_FRAMES_TO_RENDER_TIME_BALANCE;
}

bool Intermission::render_time_balance(RenderContext& ctx) {
    ctx.y_offset += text_head.getHeight() + SIZE_TEXT_HEAD / 4;
    text_rest_info.renderDirect(X_LIMIT_OPTION, ctx.y_offset,
                                "Time balance " + std::to_string(current_balance) + "s",
                                GOLDEN_YELLOW, true, DARK_VIOLET);

    render_improvements_purchase(ctx);

    return true;
}

void Intermission::render_improvements_purchase(const RenderContext& ctx) {
    int x_start_icon = X_LIMIT_OPTION + text_rest_info.getWidth() + SIZE_TEXT_HEAD / 2;
    for (auto event = improvements_purchased.begin(); event != improvements_purchased.end();) {
        int size_icon = text_rest_info.getHeight() * 0.75;
        int icon_width =
                static_cast<int>(size_icon * event->icon.getWidth() / event->icon.getHeight());
        event->icon.renderEntity(Area(0, 0, event->icon.getWidth(), event->icon.getHeight()),
                                 Area(x_start_icon, ctx.y_offset + 10, icon_width, size_icon), 0.0);
        x_start_icon += icon_width + 5;

        text_rest_info.renderDirect(x_start_icon, ctx.y_offset,
                                    "-" + std::to_string(event->cost) + "s", RED, true,
                                    DARK_VIOLET);

        x_start_icon += text_rest_info.getWidth() + SIZE_TEXT_HEAD / 2;

        if ((this->iteration - event->iteration_purchase) > AMOUNT_FRAMES_WAITING * 6) {
            event = improvements_purchased.erase(event);
        } else {
            ++event;
        }
    }
}

bool Intermission::render_improvement_options(RenderContext& ctx) {
    calculate_layout(ctx);

    int frames = ctx.iteration_phase - (AMOUNT_FRAMES_ANIMATION + AMOUNT_FRAMES_WAITING * 3);
    int n = std::min(frames / AMOUNT_FRAMES_WAITING, static_cast<int>(improvement_options.size()));

    int index = 0;
    for (int i = 0; i < n; i++) {
        auto it = selected_improvements.find(improvement_options[i].improvement_id);
        if (it != selected_improvements.end() && it->second.is_selected) {
            continue;
        }

        render_single_option(improvement_options[i], index, ctx);
        index++;
    }

    return true;
}

void Intermission::calculate_layout(RenderContext& ctx) {
    ctx.y_start_options = ctx.y_offset + text_rest_info.getHeight() * 1.5;
    ctx.y_limit_options = WINDOW_HEIGHT - SIZE_TEXT_HEAD + 40;
    ctx.option_height =
            improvement_options.size() > 0 ?
                    (ctx.y_limit_options - ctx.y_start_options) / improvement_options.size() :
                    0;
}

void Intermission::render_single_option(const ImprovementOption& option, int index,
                                        const RenderContext& ctx) {
    int y_option_index = ctx.y_start_options + index * ctx.option_height;
    int x_limit_option = X_LIMIT_OPTION;
    int offset_y = 8;

    int button_upgrade_width = static_cast<int>(SIZE_ICON_BUTTON * button_upgrade.getWidth() /
                                                button_upgrade.getHeight());
    button_upgrade.renderEntity(
            Area(0, 0, button_upgrade.getWidth(), button_upgrade.getHeight()),
            Area(x_limit_option, y_option_index, button_upgrade_width, SIZE_ICON_BUTTON), 0.0);

    show_info_center(text_head, option.key, x_limit_option, x_limit_option + button_upgrade_width,
                     y_option_index + 16, option.color, DEEP_INDIGO_VIOLET);

    x_limit_option += SIZE_TEXT_HEAD + button_upgrade_width;

    int size_icon_width =
            static_cast<int>(SIZE_ICON_BUTTON * option.icon.getWidth() / option.icon.getHeight());
    int x_offset_icon = static_cast<float>(SIZE_ICON_BUTTON - size_icon_width) *
                        option.icon.getWidth() / option.icon.getHeight();
    option.icon.renderEntity(
            Area(0, 0, option.icon.getWidth(), option.icon.getHeight()),
            Area(x_limit_option + x_offset_icon, y_option_index, size_icon_width, SIZE_ICON_BUTTON),
            0.0);

    x_limit_option += static_cast<float>(SIZE_TEXT_HEAD) * 3;

    show_info_center(text_keys, option.improvement, x_limit_option, x_limit_option + 412,
                     y_option_index + offset_y, option.color, DARK_PURPLE_VIOLET);

    x_limit_option += SIZE_TEXT_HEAD * 6;

    show_info_center(text_keys, option.description, x_limit_option,
                     WINDOW_WIDTH - static_cast<float>(SIZE_TEXT_HEAD), y_option_index + offset_y,
                     option.color, DARK_VIOLET);
}
