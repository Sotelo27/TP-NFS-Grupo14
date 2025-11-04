#include "client.h"

#include "client_game.h"
#include <iostream>
#include "../common/socket.h"
#include "QT/login_window.h"
#include <QApplication>

Client::Client(const char* hostname, const char* servname):
        server_handler(Socket(hostname, servname)),
        login_window(nullptr) {
    std::cout << "[Client] Connected to server " << hostname << ":" << servname << std::endl;
}

void Client::start() {
    std::cout << "[Client] Starting server handler threads..." << std::endl;
    server_handler.start();
    
    int n = 0;
    QApplication app(n, nullptr);
    login_window = new LoginWindow(server_handler);
    login_window->show();
    app.exec();

    ClientGame game(1, server_handler);
    game.start();
}

Client::~Client() {
    if (login_window) {
        delete login_window;
    }
}
