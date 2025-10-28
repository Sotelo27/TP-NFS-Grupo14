#ifndef CLIENT_H
#define CLIENT_H

#include <optional>
#include <string>

#include "protocol_client.h"

class Client {
private:
    ProtocolClient protocol;

public:
    explicit Client(const char* host, const char* service);

    void start();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    ~Client() = default;
};

#endif
