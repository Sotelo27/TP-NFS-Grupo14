#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <list>
#include <memory>
#include <string>
#include <utility>

#include "../common/queue.h"
#include "../common/thread.h"

#include "acceptor.h"
#include "client_handler.h"
#include "client_list.h"
#include "game.h"
#include "protocol_server.h"

class Gameloop: public Thread {
private:
    Game& game;
    ClientListProtected& clients;
    Queue<ClientAction>& actiones_clients;

    void procesar_actiones();
    void iteracion_game();

public:
    /*
     * Constructor de Gameloop que recibe un Socket ya conectado al
     * puerto de escucha del servidor, una referencia al game, una
     * referencia a la list de clients y una referencia a la cola de
     * actiones de clients.
     *
     * Este se encargará de liberar los recursos al finalizar.
     */
    explicit Gameloop(Game& game, ClientListProtected& clients,
                      Queue<ClientAction>& actiones_clients);

    /*
     * Inicia el aceptor y el loop principal del servidor.
     */
    void run() override;

    Gameloop(const Gameloop&) = delete;
    Gameloop& operator=(const Gameloop&) = delete;

    Gameloop(Gameloop&&) = default;
    Gameloop& operator=(Gameloop&&) = default;

    ~Gameloop();
};

#endif
