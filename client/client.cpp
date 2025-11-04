#include "client.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include "client_game.h"
#include "QT/login_window.h"

Client::Client(const char* host, const char* service)
    : server_handler(Socket(host, service)) {}

// void Client::start() {
//       CarSpriteID current_car = CarSpriteID::CommonGreenCar;
//       ClientGame game(current_car, host, service);
//       game.start();
// }

void Client::start() {
    LoginWindow* login = new LoginWindow(server_handler);
    login->show();
}
