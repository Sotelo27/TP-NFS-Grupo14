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

    size_t my_id;
    login_window = new LoginWindow(server_handler, my_id);
    login_window->show();
    app.exec();

    std::cout << "[Client] Starting game loop..." << std::endl;
    std::cout << "[Client] My client ID is: " << my_id << std::endl;
    ClientGame game(my_id, server_handler);
    game.start();
}

Client::~Client() {
    if (login_window) {
        delete login_window;
    }
}
