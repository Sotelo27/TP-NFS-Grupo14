#ifndef CLIENT_THREAD_SEND_H
#define CLIENT_THREAD_SEND_H

#include "client_action.h"
#include "../common/queue.h"
#include "../common/thread.h"

#include "server_protocol.h"

// struct server_msg_pos {
//     uint32_t id;
//     int16_t x{0};
//     int16_t y{0};
//     float angle{0.f};
// };

class ClientThreadSend: public Thread {
private:
    ServerProtocol& protocol;
    size_t id = 0;
    Queue<ClientMessage>& mensajes_a_enviar;

public:
    /*
     * Constructor de ClientThreadSend que recibe una referencia al
     * protocol del client, el id del client y una referencia a la
     * cola de mensajes a enviar.
     */
    explicit ClientThreadSend(ServerProtocol& protocol, size_t id,
                             Queue<ClientMessage>& mensajes_a_enviar);

    void run() override;

    ClientThreadSend(const ClientThreadSend&) = delete;
    ClientThreadSend& operator=(const ClientThreadSend&) = delete;

    ClientThreadSend(ClientThreadSend&&) = default;
    ClientThreadSend& operator=(ClientThreadSend&&) = default;

    ~ClientThreadSend() = default;
};

#endif
