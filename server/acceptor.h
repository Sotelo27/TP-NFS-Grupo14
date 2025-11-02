#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <list>
#include <memory>
#include <vector>

#include "../common/queue.h"
#include "../common/socket.h"
#include "../common/thread.h"

#include "client_handler.h"
#include "monitor_lobby.h"

class Acceptor: public Thread {
private:
    Socket skt_server;
    MonitorLobby& lobby;

public:
    /*
     * Crea un acceptor que acepta conexiones en el socket
     * pasado por referencia.
     *
     * El socket debe estar previamente creado y enlazado
     * al puerto de escucha.
     */
    Acceptor(const char* puerto, MonitorLobby& lobby);

    void run() override;

    /*
     * Se debe usar este método para detener el acceptor correctamente.
     */
    void stop_acceptor();

    Acceptor(const Acceptor&) = delete;
    Acceptor& operator=(const Acceptor&) = delete;

    Acceptor(Acceptor&&) = default;
    Acceptor& operator=(Acceptor&&) = default;

    ~Acceptor();
};

#endif
