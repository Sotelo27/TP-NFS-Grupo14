#ifndef CLIENT_H
#define CLIENT_H

#include <optional>
#include <string>
#include "../common/queue.h"
#include "../common/socket.h"
#include "connection/server_handler.h"
#include "client_protocol.h"

class LoginWindow;

class Client {
private:
    ServerHandler server_handler;
    LoginWindow* login_window;

public:
    explicit Client(const char* host, const char* service);

    void start();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    ~Client();
};

#endif
