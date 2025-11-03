#ifndef SERVER_THREAD_SEND_H
#define SERVER_THREAD_SEND_H

#include "../../common/dto/movement.h"
#include "../../common/queue.h"
#include "../../common/thread.h"
#include "../client_protocol.h"

struct client_msg_pos {
    Movement tecla;
};

class ServerThreadSend: public Thread {
private:
    ClientProtocol& protocol;
    Queue<client_msg_pos>& messages_send;

public:
    /*
     * Constructor de ServerThreadSend que recibe una referencia al
     * protocol del client, el id del client y una referencia a la
     * cola de mensajes a enviar.
     */
    explicit ServerThreadSend(ClientProtocol& protocol, Queue<client_msg_pos>& messages_send);

    void run() override;

    ServerThreadSend(const ServerThreadSend&) = delete;
    ServerThreadSend& operator=(const ServerThreadSend&) = delete;

    ServerThreadSend(ServerThreadSend&&) = default;
    ServerThreadSend& operator=(ServerThreadSend&&) = default;

    ~ServerThreadSend() = default;
};

#endif
