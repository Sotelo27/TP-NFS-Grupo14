#ifndef CLIENT_H
#define CLIENT_H

#include <optional>
#include <string>
#include "../common/queue.h"
#include "connection/server_handler.h"
#include "client_protocol.h"

class Client {
private:
    ServerHandler server_handler;

public:
    explicit Client(const char* host, const char* service);

    void start();
    // void login();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    ~Client() = default;
};

#endif
