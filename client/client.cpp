#include "client.h"

#include <iostream>
#include "QT/login_window.h"

Client::Client(const char* hostname, const char* servname):
        skt(hostname, servname),
        server_handler(std::move(skt)),
        login_window(nullptr) {
    std::cout << "[Client] Connected to server " << hostname << ":" << servname << std::endl;
}

void Client::start() {
    std::cout << "[Client] Starting server handler threads..." << std::endl;
    server_handler.start();
    
    login_window = new LoginWindow(server_handler);
    login_window->show();
}

Client::~Client() {
    if (login_window) {
        delete login_window;
    }
}
