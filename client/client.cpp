#include "client.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include "client_game.h"

Client::Client(const char* host, const char* service): host(host), service(service) {}

void Client::start() {
    CarSpriteID current_car = CarSpriteID::CommonGreenCar;
    ClientGame game(current_car, host, service);
    game.start();
}
