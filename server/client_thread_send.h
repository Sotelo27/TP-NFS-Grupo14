#ifndef CLIENT_THREAD_SEND_H
#define CLIENT_THREAD_SEND_H

#include "../common/queue.h"
#include "../common/thread.h"

#include "protocol_server.h"

struct server_msg {
    uint16_t cantidad_nitros_activos;
    uint8_t mensaje;
};

class ClientThreadSend: public Thread {
private:
    ProtocolServer& protocol;
    size_t id = 0;
    Queue<server_msg>& mensajes_a_enviar;

public:
    /*
     * Constructor de ClientThreadSend que recibe una referencia al
     * protocol del client, el id del client y una referencia a la
     * cola de mensajes a enviar.
     */
    explicit ClientThreadSend(ProtocolServer& protocol, size_t id,
                             Queue<server_msg>& mensajes_a_enviar);

    void run() override;

    ClientThreadSend(const ClientThreadSend&) = delete;
    ClientThreadSend& operator=(const ClientThreadSend&) = delete;

    ClientThreadSend(ClientThreadSend&&) = default;
    ClientThreadSend& operator=(ClientThreadSend&&) = default;

    ~ClientThreadSend() = default;
};

#endif
