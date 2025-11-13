#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "sdl_wrappers/SdlDrawFill.h"

#include "constants.h"

ClientGame::ClientGame(size_t client_id, ServerHandler& server_handler, bool& game_is_over) :
        client_id(client_id),
        server_handler(server_handler),
        game_is_over(game_is_over),
        running(false),
        src_area_map(0, 0, 0, 0),
        dest_area_map(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
        info_players(),
        map_name("LibertyCity") // <-- NUEVO: valor por defecto
{}

void ClientGame::start() {
    this->running = true;

    // Esperar mensaje RACE_START antes de cargar el mapa
    while (true) {
        ServerMessage msg = server_handler.recv_response_from_server();
        if (msg.type == ServerMessage::Type::RaceStart) {
            map_name = msg.map_name.empty() ? "LibertyCity" : msg.map_name; // <-- CAMBIA username por map_name
            break;
        }
        // Puedes agregar lógica para salir si hay error/desconexión
    }

    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    CarSpriteSheet car_sprites(window);

    // Convertir map_name a MapID
    MapID map_id;
    if (map_name == "SanAndreas") {
        map_id = MapID::SanAndreas;
    } else if (map_name == "LibertyCity") {
        map_id = MapID::LibertyCity;
    } else if (map_name == "ViceCity") {
        map_id = MapID::ViceCity;
    } else {
        map_id = MapID::LibertyCity; // default
    }

    MapsTextures map_manager(window);
    map_manager.loadMap(map_id);

    std::cout << "[ClientGame] Tamaño real del mapa: " << map_manager.getCurrentMapWidth() << "x"
              << map_manager.getCurrentMapHeight() << std::endl;

    GameHud game_hud(window, map_manager, client_id, info_players, car_sprites);

    std::cout << "[ClientGame] Juego iniciado, esperando posiciones del servidor..." << std::endl;
    while (this->running) {
        update_state_from_position();

        // Clear display
        window.fill();

        update_animation_frames(map_manager, car_sprites);

        render_in_z_order(window, map_manager, car_sprites, game_hud);

        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }
}

void ClientGame::update_state_from_position() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                const SDL_KeyboardEvent& keyEvent = (SDL_KeyboardEvent&)event;
                switch (keyEvent.keysym.sym) {
                    case SDLK_LEFT:
                        server_handler.send_movement(Movement::Left);
                        std::cout << "[ClientGame] LEFT pressed\n";
                        break;
                    case SDLK_RIGHT:
                        server_handler.send_movement(Movement::Right);
                        std::cout << "[ClientGame] RIGHT pressed\n";
                        break;
                    case SDLK_UP:
                        server_handler.send_movement(Movement::Up);
                        std::cout << "[ClientGame] UP pressed\n";
                        break;
                    case SDLK_DOWN:
                        server_handler.send_movement(Movement::Down);
                        std::cout << "[ClientGame] DOWN pressed\n";
                        break;
                }
            } break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_QUIT:
                std::cout << "[ClientGame] Quit event received" << std::endl;
                running = false;

                // esto se debe borrar cuando se maneje el fin del juego
                game_is_over = true;
                
                break;
        }
    }

    const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
    if (keyboard[SDL_SCANCODE_LEFT]) {
        server_handler.send_movement(Movement::Left);
    }
    if (keyboard[SDL_SCANCODE_RIGHT]) {
        server_handler.send_movement(Movement::Right);
    }
    if (keyboard[SDL_SCANCODE_UP]) {
        server_handler.send_movement(Movement::Up);
    }
    if (keyboard[SDL_SCANCODE_DOWN]) {
        server_handler.send_movement(Movement::Down);
    }

    // Procesar mensajes del servidor
    bool keep_loop = true;
    int msg_count = 0;
    static int frame_count = 0;
    frame_count++;

    while (keep_loop && msg_count < 10) {
        ServerMessage action = server_handler.recv_response_from_server();

        if (action.type == ServerMessage::Type::MapInfo) {
            info_players.clear();

            for (const auto& p_info: action.players_tick) {
                info_players[p_info.player_id] = CarInfoGame{p_info, Area()};
            }
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
            this->running = false;
            std::cout << "[ClientGame] Received Unknown message from server, probably disconnected. Exiting..."
                      << std::endl;
        }
        // se debería recibir el fin del juego
        // game_is_over = true;

        msg_count++;
    }
}

void ClientGame::update_map_area(const MapsTextures& map_manager) {
    const PlayerTickInfo& info_my_car = info_players[client_id].info_car;

    int x_map = info_my_car.x - MAP_WIDTH_SIZE / 2;
    int y_map = info_my_car.y - MAP_HEIGHT_SIZE / 2;

    if (x_map < 0) {
        x_map = 0;
    }

    if (y_map < 0) {
        y_map = 0;
    }

    if (x_map > map_manager.getCurrentMapWidth() - MAP_WIDTH_SIZE) {
        x_map = map_manager.getCurrentMapWidth() - MAP_WIDTH_SIZE;
    }

    if (y_map > map_manager.getCurrentMapHeight() - MAP_HEIGHT_SIZE) {
        y_map = map_manager.getCurrentMapHeight() - MAP_HEIGHT_SIZE;
    }

    src_area_map.update(x_map, y_map, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);
}

void ClientGame::update_animation_frames(const MapsTextures& map_manager,
                                         const CarSpriteSheet& car_sprites) {
    update_map_area(map_manager);

    Area extend_area_map(src_area_map.getX() - CAR_WIDTH_LARGE,
                         src_area_map.getY() - CAR_HEIGHT_LARGE,
                         src_area_map.getWidth() + CAR_WIDTH_LARGE * 2,
                         src_area_map.getHeight() + CAR_HEIGHT_LARGE * 2);

    for (auto& [id, car]: info_players) {
        if (car.info_car.x < extend_area_map.getX() ||
            car.info_car.x > extend_area_map.getX() + extend_area_map.getWidth() ||
            car.info_car.y < extend_area_map.getY() ||
            car.info_car.y > extend_area_map.getY() + extend_area_map.getHeight()) {
            continue;
        }

        // el auto debería de pasarlo el server en algún momento
        CarData car_data = car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        int x_car_screen = (car.info_car.x - src_area_map.getX()) * MAP_TO_VIEWPORT_SCALE_X;
        int y_car_screen = (car.info_car.y - src_area_map.getY()) * MAP_TO_VIEWPORT_SCALE_Y;
        car.dest_area.update(x_car_screen - car_data.width_scale_screen / 2,
                             y_car_screen - car_data.height_scale_screen / 2,
                             car_data.width_scale_screen, car_data.height_scale_screen);
    }
}

void ClientGame::render_cars(const CarSpriteSheet& car_sprites) {
    for (const auto& [id, car]: info_players) {
        if (car.dest_area.getWidth() == 0 || car.dest_area.getHeight() == 0) {
            continue;
        }

        const CarData& car_data =
                car_sprites.getCarData(static_cast<CarSpriteID>(car.info_car.car_id));

        car_sprites.render(car_data.area, car.dest_area, car.info_car.angle);
    }
}

void ClientGame::render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                                   const CarSpriteSheet& car_sprites, GameHud& game_hud) {

    map_manager.render(src_area_map, dest_area_map);

    render_cars(car_sprites);

    game_hud.render();

    window.render();
}

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

    info_players.clear();
}
