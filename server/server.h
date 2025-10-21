#ifndef SERVER_H
#define SERVER_H

#include <list>
#include <memory>
#include <string>
#include <utility>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "acceptor.h"
#include "client_handler.h"
#include "client_list.h"
#include "gameloop.h"
#include "game.h"
#include "protocol_server.h"

class Server {
private:
    Game game;
    ClientListProtected clients;
    Queue<ClientAction> actiones_clients;
    Acceptor acceptor;
    Gameloop gameloop;

public:
    /*
     * Constructor de Server que recibe un puerto al que enlazarse.
     */
    explicit Server(const char* puerto);

    /*
     * Inicia el aceptor y el loop principal del servidor.
     */
    void start();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    Server(Server&&) = default;
    Server& operator=(Server&&) = default;

    ~Server();
};

#endif
