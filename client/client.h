#ifndef CLIENT_H
#define CLIENT_H

#include <optional>
#include <string>
#include "../common/queue.h"
#include "../common/socket.h"
#include "connection/server_handler.h"
#include "client_protocol.h"
#include "QT/game_window.h"

class LoginWindow;

class Client {
private:
    ServerHandler server_handler;
    GameWindow* game_window_start;
    GameWindow* game_window_end;

    void open_game_window(size_t& my_id, ServerHandler& server_handler, bool& game_is_over);
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
