#include "intermission.h"

#include "../constants.h"
#include "../utils/rgb.h"

#define BACKGROUND_IMAGE_PATH std::string(ASSETS_PATH) + "/images/fondo_cars.jpg"
#define SIZE (static_cast<float>(WINDOW_HEIGHT) + WINDOW_WIDTH) / 27.27
#define AMOUNT_FRAMES_ANIMATION 90

Intermission::Intermission(SdlWindow& window, ServerHandler& server_handler, bool& main_running):
        ConstantRateLoop(FRAME_RATE),
        window(window),
        server_handler(server_handler),
        main_running(main_running),
        cheat_detector(5),
        iterations(1),
        background_texture(BACKGROUND_IMAGE_PATH, window, Rgb(0, 255, 0)),
        text(FONT_STYLE_PX, SIZE, window) {}

void Intermission::function() {
    handle_sdl_events();

    process_server_messages(ServerMessage::Type::Empty, 10);

    show_results();

    window.render();
}

void Intermission::run() {
    iterations = 1;
    ConstantRateLoop::start_loop();
}

void Intermission::show_results() {
    std::list<PlayerInfoI> dummy;  // en un futuro se recibe la lista de jugadores
    dummy.push_back({1, "Player1", 120, 300});
    dummy.push_back({2, "Player2", 150, 320});
    dummy.push_back({3, "Player3", 180, 350});
    dummy.push_back({4, "Player4", 200, 400});
    dummy.push_back({5, "Player5", 220, 450});
    dummy.push_back({6, "Player6", 250, 500});
    dummy.push_back({7, "Player7", 300, 600});
    dummy.push_back({8, "Player8", 350, 700});

    if (iterations <= AMOUNT_FRAMES_ANIMATION) {
        int y_animation = (background_texture.getHeight() * iterations) / AMOUNT_FRAMES_ANIMATION;
        int y_window = (WINDOW_HEIGHT * iterations) / AMOUNT_FRAMES_ANIMATION;
        background_texture.renderEntity(
                Area(0, y_animation, background_texture.getWidth(), background_texture.getHeight()),
                Area(0, 0, WINDOW_WIDTH, y_window), 0.0);
    } else if (iterations > AMOUNT_FRAMES_ANIMATION + 30) {
        text.renderDirect(SIZE, SIZE, "POSITion 123 45 67890", Rgb(0, 255, 0), true,
                          Rgb(255, 0, 0));
    }
    iterations++;  // esto se saca cuando se use el del bucle principal
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
    }
}

void Intermission::handle_sdl_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;

                const char* keyName = SDL_GetKeyName(keyEvent.keysym.sym);
                std::cout << "Tecla presionada: " << keyName << std::endl;

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
