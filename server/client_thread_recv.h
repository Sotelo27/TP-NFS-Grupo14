#ifndef CLIENT_THREAD_RECV_H
#define CLIENT_THREAD_RECV_H

#include "../common/queue.h"
#include "../common/thread.h"

#include "client_action.h"
#include "protocol_server.h"


class ClientThreadRecv: public Thread {
private:
    ProtocolServer& protocol;
    size_t id = 0;
    Queue<ClientAction>& actiones_clients;

public:
    /*
     * Constructor de ClientThreadRecv que recibe una referencia al
     * protocol del client, el id del client y una referencia a la
     * cola de actiones de clients, donde se pushearán las peticiones
     * recibidas.
     */
    explicit ClientThreadRecv(ProtocolServer& protocol, size_t id,
                             Queue<ClientAction>& actiones_clients);

    void run() override;

    ClientThreadRecv(const ClientThreadRecv&) = delete;
    ClientThreadRecv& operator=(const ClientThreadRecv&) = delete;

    ClientThreadRecv(ClientThreadRecv&&) = default;
    ClientThreadRecv& operator=(ClientThreadRecv&&) = default;

    ~ClientThreadRecv() = default;
};

#endif
