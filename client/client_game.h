#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <optional>
#include <string>

#include "utils/car_sprite_sheet.h"

class ClientGame {
private:
    bool running;
    CarSpriteID current_car;

public:
    explicit ClientGame(CarSpriteID car);

    void start();

    ClientGame(const ClientGame&) = delete;
    ClientGame& operator=(const ClientGame&) = delete;

    ClientGame(ClientGame&&) = default;
    ClientGame& operator=(ClientGame&&) = default;

    ~ClientGame() = default;
};

#endif
