#include "client_game.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include <SDL2/SDL.h>

#include "constants.h"

ClientGame::ClientGame(size_t client_id, ServerHandler& server_handler):
        client_id(client_id),
        server_handler(server_handler),
        running(false),
        positions(),
        src_area_map(0, 0, 0, 0),
        dest_area_map(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
        map_dest_areas() {}

void ClientGame::start() {
    this->running = true;

    // init resources
    SdlWindow window(WINDOW_WIDTH, WINDOW_HEIGHT);
    window.fill();

    CarSpriteSheet car_sprites(window);
    AddText add_text(24, window);

    MapsTextures map_manager(window);
    map_manager.loadMap(MapID::LibertyCity);

    const MapData& map_data = map_manager.getCurrentMapData();
    std::cout << "[ClientGame] Tamaño real del mapa: " << map_data.width_scale_screen << "x"
              << map_data.height_scale_screen << std::endl;

    std::cout << "[ClientGame] Juego iniciado, esperando posiciones del servidor..." << std::endl;
    while (this->running) {
        update_state_from_position();

        // Clear display
        window.fill();

        update_animation_frames(map_data, car_sprites);

        render_in_z_order(window, map_manager, car_sprites, add_text);

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
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
            }
            break;
            case SDL_MOUSEMOTION:
                break;
            case SDL_QUIT:
                std::cout << "[ClientGame] Quit event received" << std::endl;
                running = false;
                break;
        }
    }

    // Procesar mensajes del servidor
    bool keep_loop = true;
    int msg_count = 0;
    static int frame_count = 0;
    frame_count++;
    
    while (keep_loop && msg_count < 10) {
        ServerMessage action = server_handler.recv_response_from_server();
        
        if (action.type == ServerMessage::Type::Pos) {
            // Actualizar posición si es para este cliente
            if (action.id == client_id) {
                // Logear cada 60 frames (1 vez por segundo aprox)
                if (frame_count % 60 == 0) {
                    std::cout << "[ClientGame] Server position: (" << action.x << ", " << action.y 
                              << ") - Current position: (" << positions.x_car_map << ", " << positions.y_car_map << ")\n";
                }
                
                // SOLO actualizar si las posiciones del servidor son válidas (no son 0,0)
                if (action.x != 0 || action.y != 0) {
                    positions.x_car_map = action.x;
                    positions.y_car_map = action.y;
                }
            }
        } else if (action.type == ServerMessage::Type::Unknown) {
            keep_loop = false;
        }
        msg_count++;
    }
}

void ClientGame::update_animation_frames(const MapData& map_data,
                                         const CarSpriteSheet& car_sprites) {
    int x_map = positions.x_car_map - MAP_WIDTH_SIZE / 2;
    int y_map = positions.y_car_map - MAP_HEIGHT_SIZE / 2;

    int x_car_screen = WINDOW_WIDTH / 2;
    int y_car_screen = WINDOW_HEIGHT / 2;

    if (x_map < 0) {
        x_car_screen = positions.x_car_map * MAP_TO_VIEWPORT_SCALE_X;
        x_map = 0;
    }

    if (y_map < 0) {
        y_car_screen = positions.y_car_map * MAP_TO_VIEWPORT_SCALE_Y;
        y_map = 0;
    }

    if (x_map > map_data.width_scale_screen - MAP_WIDTH_SIZE) {
        x_map = map_data.width_scale_screen - MAP_WIDTH_SIZE;
        x_car_screen = (positions.x_car_map - x_map) * MAP_TO_VIEWPORT_SCALE_X;
    }

    if (y_map > map_data.height_scale_screen - MAP_HEIGHT_SIZE) {
        y_map = map_data.height_scale_screen - MAP_HEIGHT_SIZE;
        y_car_screen = (positions.y_car_map - y_map) * MAP_TO_VIEWPORT_SCALE_Y;
    }

    src_area_map.update(x_map, y_map, MAP_WIDTH_SIZE, MAP_HEIGHT_SIZE);

    CarData car_data = car_sprites.getCarData(this->current_car);
    map_dest_areas[client_id] = Area(x_car_screen - car_data.width_scale_screen / 2,
                                     y_car_screen - car_data.height_scale_screen / 2,
                                     car_data.width_scale_screen, car_data.height_scale_screen);
}

void ClientGame::render_in_z_order(SdlWindow& window, const MapsTextures& map_manager,
                                   const CarSpriteSheet& car_sprites, const AddText& add_text) {
    const CarData& car_data = car_sprites.getCarData(this->current_car);

    map_manager.render(src_area_map, dest_area_map);
    
    car_sprites.render(car_data.area, map_dest_areas[client_id]);
    
    std::string client_id_str = "Client ID: " + std::to_string(client_id);
    add_text.renderText(client_id_str, Rgb(255, 255, 255, 255), Area(10, 10, 0, 0));

    window.render();
}

ClientGame::~ClientGame() {
    if (server_handler.is_alive()) {
        server_handler.hard_kill();
    }

    map_dest_areas.clear();
}
