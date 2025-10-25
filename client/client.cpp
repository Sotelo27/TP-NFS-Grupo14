#include "client.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include "client_game.h"

Client::Client(const char* host, const char* service): protocol(std::move(Socket(host, service))) {}

void Client::start() {
    CarSpriteID current_car = CarSpriteID::CommonGreenCar;
    ClientGame game(current_car);
    game.start();
}
