#include "client.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include "client_game.h"

Client::Client(const char* host, const char* service): host(host), service(service) {}

void Client::start() {
    CarSpriteID current_car = CarSpriteID::CommonGreenCar;
    size_t client_id = 1;
    ClientGame game(current_car, client_id, host, service);
    game.start();
}
