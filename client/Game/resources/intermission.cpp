#include "intermission.h"

#include <algorithm>
#include <string>
#include <utility>

#include "../constants.h"
#include "../utils/rgb.h"

#define BACKGROUND_INFO_IMAGE_PATH std::string(ASSETS_PATH) + "/images/fondo_cars.jpg"
#define BACKGROUND_IMPROVEMENT_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/garaje.png"
#define BUTTON_UPGRADE_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/boton_u.png"
#define ICON_CONTROLLABILITY_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/control.png"

#define SIZE_TEXT_HEAD (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5
#define SIZE_TEXT_POSITION (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5
#define SIZE_TEXT_REST_INFO (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5

#define SIZE_ICON_BUTTON (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5

#define WHITE Rgb(255, 255, 255)

#define DARK_VIOLET Rgb(45, 0, 75)
#define VIBRANT_VIOLET Rgb(140, 0, 150)
#define NEON_MAGENTA Rgb(215, 40, 185)
#define SOFT_NEON_PINK Rgb(255, 110, 180)

#define GLITCH_VIOLET Rgb(100, 0, 255)
#define PURPLE_NEON_BRIGHT Rgb(180, 0, 255)
#define MAGENTA_VIBRANT Rgb(255, 0, 200)
#define ELECTRIC_PINK Rgb(255, 100, 255)

#define ORANGE_SUNSET Rgb(255, 125, 0)
#define ORANGE_SUN Rgb(255, 140, 0)

#define ELECTRIC_CYAN Rgb(0, 255, 255)  // a
#define NEON_LIME Rgb(150, 255, 0)      // a
#define NEON_YELLOW Rgb(255, 255, 0)    // a
#define NEON_RED Rgb(255, 0, 0)

#define VIBRANT_ORANGE Rgb(255, 100, 0)      // a
#define BLUE Rgb(0, 0, 255)                  // a
#define RED Rgb(255, 0, 0)                   // a
#define GLITCH_LIGHT_BLUE Rgb(50, 150, 255)  // a

#define NEON_YO Rgb(255, 200, 0)

constexpr int AMOUNT_FRAMES_ANIMATION = 90;
constexpr int AMOUNT_FRAMES_WAITING = 20;
constexpr int RESULTS = AMOUNT_FRAMES_ANIMATION + 2 * AMOUNT_FRAMES_WAITING;

#define KEY_NEXT_BUTTON "N"
const char NEXT_PHASE_TEXT[] = "Press " KEY_NEXT_BUTTON " to continue...";

const char KEY_IMPROVEMENT_SPEED = 'S';
const char KEY_IMPROVEMENT_HEALTH = 'H';
const char KEY_IMPROVEMENT_ACCELERATION = 'A';
const char KEY_IMPROVEMENT_MASS = 'M';
const char KEY_IMPROVEMENT_CONTROLLABILITY = 'C';

Intermission::Intermission(SdlWindow& window, ServerHandler& server_handler,
                           MapsTextures& map_manager, bool& main_running):
        ConstantRateLoop(FRAME_RATE),
        window(window),
        server_handler(server_handler),
        map_manager(map_manager),
        main_running(main_running),
        cheat_detector(5),
        background_info(BACKGROUND_INFO_IMAGE_PATH, window, Rgb(0, 255, 0)),
        background_improvement(BACKGROUND_IMPROVEMENT_IMAGE_PATH, window, Rgb(0, 255, 0)),
        button_upgrade(BUTTON_UPGRADE_IMAGE_PATH, window,
                       Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        icon_controllability(ICON_CONTROLLABILITY_IMAGE_PATH, window,
                             Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        text_head(FONT_STYLE_PX, SIZE_TEXT_HEAD, window),
        text_position(FONT_STYLE_VS1, SIZE_TEXT_POSITION, window),
        text_rest_info(FONT_STYLE_CC, SIZE_TEXT_REST_INFO, window),
        text_keys(FONT_STYLE_AA, SIZE_TEXT_REST_INFO, window),
        improvement_phase(false),
        iteration_init_improvement_phase(0),
        player_infos(),
        improvement_options() {
    improvement_options.push_back({std::string(1, KEY_IMPROVEMENT_HEALTH), icon_controllability,
                                   "Health", "Survive more hits", true});
    improvement_options.push_back({std::string(1, KEY_IMPROVEMENT_SPEED), icon_controllability,
                                   "Speed", "Higher maximum speed", true});
    improvement_options.push_back({std::string(1, KEY_IMPROVEMENT_CONTROLLABILITY),
                                   icon_controllability, "Controllability", "Better turning",
                                   true});
    improvement_options.push_back({std::string(1, KEY_IMPROVEMENT_ACCELERATION),
                                   icon_controllability, "Acceleration", "Quicker 0-100 km/h",
                                   true});
    improvement_options.push_back({std::string(1, KEY_IMPROVEMENT_MASS), icon_controllability,
                                   "Mass", "Stronger collisions", true});
}

void Intermission::function() {
    handle_sdl_events();

    process_server_messages(ServerMessage::Type::Empty, 10);

    window.fill();

    show_results();
    if (improvement_phase) {
        show_improvement_phase();
    }

    window.render();
}

void Intermission::run(std::vector<PlayerResultCurrent> player_infos) {
    std::sort(player_infos.begin(), player_infos.end(),
              [](const PlayerResultCurrent& a, const PlayerResultCurrent& b) {
                  return a.position < b.position;
              });
    this->player_infos = std::move(player_infos);

    improvement_phase = false;
    ConstantRateLoop::start_loop();
}

void Intermission::show_info_center(SdlFont& font, const std::string& info, int x_start, int x_end,
                                    int y_info, const Rgb& color_front, const Rgb& color_shadow) {
    font.loadText(info, WHITE, true);
    int x_position_center = x_start + (x_end - x_start) / 2 - font.getWidth() / 2;
    font.renderDirect(x_position_center, y_info, info, color_front, true, color_shadow);
}

void Intermission::show_table_results() {
    if (iteration <= AMOUNT_FRAMES_ANIMATION + AMOUNT_FRAMES_WAITING) {
        return;
    }

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

    // en un futuro el podio va a estar con colores especiales

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

    show_table_results();

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
            map_manager.loadMap(static_cast<MapID>(action.map_id));
            this->running = false;
            keep_loop = false;
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
            this->running = false;
            std::cout << "[ClientGame] Received Unknown message from server, probably "
                         "disconnected. Exiting..."
                      << std::endl;
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
    int iteration_phase = iteration - iteration_init_improvement_phase;

    int background_frame_limit = std::min(iteration_phase, AMOUNT_FRAMES_ANIMATION);
    int y_animation =
            (background_improvement.getHeight() * background_frame_limit) / AMOUNT_FRAMES_ANIMATION;
    int y_window = (WINDOW_HEIGHT * background_frame_limit) / AMOUNT_FRAMES_ANIMATION;
    background_improvement.renderEntity(
            Area(0, background_improvement.getHeight() - y_animation,
                 background_improvement.getWidth(), background_improvement.getHeight()),
            Area(0, 0, WINDOW_WIDTH, y_window), 0.0);

    show_info_center(text_head, "CAR UPGRADE", SIZE_TEXT_HEAD, WINDOW_WIDTH - SIZE_TEXT_HEAD,
                     SIZE_TEXT_HEAD, NEON_YO, DARK_VIOLET);

    int y_start_options = SIZE_TEXT_HEAD * 2 + text_head.getHeight();
    int y_limit_options = WINDOW_HEIGHT - SIZE_TEXT_HEAD;

    int option_height = improvement_options.size() > 0 ?
                                (y_limit_options - y_start_options) / improvement_options.size() :
                                0;

    int i = 0;
    for (const ImprovementOption& option: improvement_options) {
        int y_option_index = y_start_options + i * option_height;

        int x_limit_option = static_cast<float>(SIZE_TEXT_HEAD) * 7 / 4;
        int button_upgrade_width = static_cast<int>(SIZE_ICON_BUTTON * button_upgrade.getWidth() /
                                                    button_upgrade.getHeight());
        button_upgrade.renderEntity(
                Area(0, 0, button_upgrade.getWidth(), button_upgrade.getHeight()),
                Area(x_limit_option, y_option_index, button_upgrade_width, SIZE_ICON_BUTTON), 0.0);
        show_info_center(text_head, option.key, x_limit_option,
                         x_limit_option + button_upgrade_width, y_option_index + 8, RED,
                         DARK_VIOLET);

        x_limit_option += SIZE_TEXT_HEAD + button_upgrade_width;
        option.icon.renderEntity(Area(0, 0, option.icon.getWidth(), option.icon.getHeight()),
                                 Area(x_limit_option, y_option_index,
                                      static_cast<int>(SIZE_ICON_BUTTON * option.icon.getWidth() /
                                                       option.icon.getHeight()),
                                      SIZE_ICON_BUTTON),
                                 0.0);

        x_limit_option += static_cast<float>(SIZE_TEXT_HEAD) * 3;
        show_info_center(text_keys, option.improvement, x_limit_option, x_limit_option + 412,
                         y_option_index, BLUE, DARK_VIOLET);

        x_limit_option += SIZE_TEXT_HEAD * 6;
        show_info_center(text_keys, option.description, x_limit_option,
                         WINDOW_WIDTH - static_cast<float>(SIZE_TEXT_HEAD), y_option_index, RED,
                         DARK_VIOLET);

        i++;
    }
}
