#include "intermission.h"

#include <algorithm>
#include <string>

#include "../constants.h"
#include "../utils/rgb.h"

#define BACKGROUND_INFO_IMAGE_PATH std::string(ASSETS_PATH) + "/images/fondo_cars.jpg"
#define BACKGROUND_IMPROVEMENT_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/garaje.png"
#define NEXT_BUTTON_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/boton_o.png"

#define SIZE_TEXT_HEAD (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5
#define SIZE_TEXT_POSITION (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5
#define SIZE_TEXT_REST_INFO (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 37.5

#define SIZE_NEXT_BUTTON (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 15.0

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

#define ELECTRIC_CYAN Rgb(0, 255, 255)
#define NEON_LIME Rgb(150, 255, 0)
#define NEON_YELLOW Rgb(255, 255, 0)
#define NEON_RED Rgb(255, 0, 0)

#define NEON_YO Rgb(255, 200, 0)

constexpr int AMOUNT_FRAMES_ANIMATION = 90;
constexpr int AMOUNT_FRAMES_WAITING = 30;
constexpr int RESULTS = AMOUNT_FRAMES_ANIMATION + 2 * AMOUNT_FRAMES_WAITING;

const char NEXT_BUTTON_TEXT[] = "Next";
const char KEY_NEXT_BUTTON = NEXT_BUTTON_TEXT[0];

Intermission::Intermission(SdlWindow& window, ServerHandler& server_handler, bool& main_running):
        ConstantRateLoop(FRAME_RATE),
        window(window),
        server_handler(server_handler),
        main_running(main_running),
        cheat_detector(5),
        background_info(BACKGROUND_INFO_IMAGE_PATH, window, Rgb(0, 255, 0)),
        background_improvement(BACKGROUND_IMPROVEMENT_IMAGE_PATH, window, Rgb(0, 255, 0)),
        next_button(NEXT_BUTTON_IMAGE_PATH, window,
                    Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)),
        text_head(FONT_STYLE_PX, SIZE_TEXT_HEAD, window),
        text_position(FONT_STYLE_VS1, SIZE_TEXT_POSITION, window),
        text_rest_info(FONT_STYLE_CC, SIZE_TEXT_REST_INFO, window),
        improvement_phase(false),
        iteration_init_improvement_phase(0) {}

void Intermission::function() {
    handle_sdl_events();

    process_server_messages(ServerMessage::Type::Empty, 10);

    if (!improvement_phase) {
        show_results();
    } else {
        show_improvement_phase();
    }

    window.render();
}

void Intermission::run() {
    improvement_phase = false;
    ConstantRateLoop::start_loop();
}

void Intermission::show_info_center(SdlFont& font, const std::string& info, int x_start, int x_end,
                                    int y_info, const Rgb& color_front, const Rgb& color_shadow) {
    font.loadText(info, WHITE, true);
    int x_position_center = x_start + (x_end - x_start) / 2 - font.getWidth() / 2;
    font.renderDirect(x_position_center, y_info, info, color_front, true, color_shadow);
}

void Intermission::show_table_results(const std::vector<PlayerInfoI>& player_infos) {
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
        const PlayerInfoI& player_info = player_infos[i];
        int y_info = y_limit + (SIZE_TEXT_REST_INFO + SIZE_TEXT_HEAD / 8) * i;

        show_info_center(text_position, std::to_string(player_info.position), x_postion_start,
                         x_position_end, y_info, WHITE, GLITCH_VIOLET);

        show_info_center(text_rest_info, player_info.name, x_player_start, x_player_end, y_info,
                         WHITE, PURPLE_NEON_BRIGHT);

        show_info_center(text_rest_info, std::to_string(player_info.race_time_seconds) + "s",
                         x_race_time_start, x_race_time_end, y_info, WHITE, MAGENTA_VIBRANT);

        show_info_center(text_rest_info, std::to_string(player_info.total_time_seconds) + "s",
                         x_total_time_start, x_total_time_end, y_info, WHITE, ELECTRIC_PINK);
    }
}

void Intermission::show_button_next() {
    int size_width = SIZE_NEXT_BUTTON;
    int size_height = (size_width * next_button.getHeight()) / next_button.getWidth();
    int next_button_x = WINDOW_WIDTH - size_width - SIZE_TEXT_HEAD;
    int next_button_y = WINDOW_HEIGHT - size_height - SIZE_TEXT_HEAD;
    next_button.renderEntity(Area(0, 0, next_button.getWidth(), next_button.getHeight()),
                             Area(next_button_x, next_button_y, size_width, size_height), 0.0);

    int x_start = next_button_x;
    int x_end = next_button_x + size_width;
    int y_info = next_button_y + SIZE_NEXT_BUTTON / 10;
    text_head.loadText(NEXT_BUTTON_TEXT, WHITE, true);
    int x_position_center = x_start + (x_end - x_start) / 2 - text_head.getWidth() / 2;
    text_head.renderDirect(x_position_center, y_info, NEXT_BUTTON_TEXT, WHITE, true,
                           SOFT_NEON_PINK);

    text_head.renderDirect(x_position_center, y_info, std::string(1, KEY_NEXT_BUTTON), NEON_YELLOW,
                           true, WHITE);
}

void Intermission::show_results() {
    std::vector<PlayerInfoI> player_infos;  // en un futuro se recibe la lista de jugadores
    player_infos.push_back({1, "Player1", 120, 300});
    player_infos.push_back({2, "Player2", 150, 320});
    player_infos.push_back({3, "Player3", 180, 350});
    player_infos.push_back({4, "Player4", 200, 400});
    player_infos.push_back({5, "Player5", 220, 450});
    player_infos.push_back({6, "Player6", 250, 500});
    player_infos.push_back({7, "Player7", 300, 600});
    player_infos.push_back({8, "Player8", 350, 700});
    // falta atajar que el string no se pase del ancho de la pantalla

    std::sort(player_infos.begin(), player_infos.end(),
              [](const PlayerInfoI& a, const PlayerInfoI& b) { return a.position < b.position; });

    if (iteration <= AMOUNT_FRAMES_ANIMATION) {
        int y_animation = (background_info.getHeight() * iteration) / AMOUNT_FRAMES_ANIMATION;
        int y_window = (WINDOW_HEIGHT * iteration) / AMOUNT_FRAMES_ANIMATION;
        background_info.renderEntity(
                Area(0, y_animation, background_info.getWidth(), background_info.getHeight()),
                Area(0, 0, WINDOW_WIDTH, y_window), 0.0);
    } else if (iteration > AMOUNT_FRAMES_ANIMATION + AMOUNT_FRAMES_WAITING) {
        show_table_results(player_infos);

        float frames = iteration - RESULTS;
        if ((frames / AMOUNT_FRAMES_WAITING) < 9) {
            return;
        }

        show_button_next();
    }
}

void Intermission::process_server_messages(ServerMessage::Type expected_type, int msg_limit) {
    int msg_count = 0;
    bool keep_loop = true;

    while (keep_loop && (msg_limit == -1 || msg_count < msg_limit)) {
        ServerMessage action = server_handler.recv_response_from_server();

        if (action.type == ServerMessage::Type::RaceStart) {
            // current_map_id = static_cast<MapID>(action.map_id);
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

void Intermission::handle_sdl_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;

                const char* keyName = SDL_GetKeyName(keyEvent.keysym.sym);

                if (keyName == std::string(1, KEY_NEXT_BUTTON)) {
                    iteration_init_improvement_phase =
                            improvement_phase ? iteration_init_improvement_phase : iteration;
                    improvement_phase = true;
                }

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
    if (iteration_phase <= AMOUNT_FRAMES_ANIMATION) {
        int y_animation =
                (background_improvement.getHeight() * iteration_phase) / AMOUNT_FRAMES_ANIMATION;
        int y_window = (WINDOW_HEIGHT * iteration_phase) / AMOUNT_FRAMES_ANIMATION;
        background_improvement.renderEntity(
                Area(0, background_improvement.getHeight() - y_animation,
                     background_improvement.getWidth(), background_improvement.getHeight()),
                Area(0, 0, WINDOW_WIDTH, y_window), 0.0);
    }
}
