#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <optional>
#include <string>

#include "connection/server_handler.h"
#include "utils/car_sprite_sheet.h"

struct Positions {
    int x_car_map;
    int y_car_map;
    int x_car_screen;
    int y_car_screen;
};

class ClientGame {
private:
    CarSpriteID current_car;
    Queue<ServerMessage> server_actions;
    ServerHandler server_handler;
    bool running;
    Positions positions;


    void update_position();

public:
    explicit ClientGame(CarSpriteID current_car, const char* host, const char* service);

    void start();

    ClientGame(const ClientGame&) = delete;
    ClientGame& operator=(const ClientGame&) = delete;

    ClientGame(ClientGame&&) = default;
    ClientGame& operator=(ClientGame&&) = default;

    ~ClientGame();
};

#endif
