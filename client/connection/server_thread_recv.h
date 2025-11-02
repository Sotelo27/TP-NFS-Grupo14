#ifndef SERVER_THREAD_RECV_H
#define SERVER_THREAD_RECV_H

#include "../../common/dto/server_msg.h"
#include "../../common/queue.h"
#include "../../common/thread.h"
#include "../client_protocol.h"

class ServerThreadRecv: public Thread {
private:
    ClientProtocol& protocol;
    Queue<ServerMessage>& server_actions;

public:
    explicit ServerThreadRecv(ClientProtocol& protocol, Queue<ServerMessage>& server_actions);

    void run() override;

    ServerThreadRecv(const ServerThreadRecv&) = delete;
    ServerThreadRecv& operator=(const ServerThreadRecv&) = delete;

    ServerThreadRecv(ServerThreadRecv&&) = default;
    ServerThreadRecv& operator=(ServerThreadRecv&&) = default;

    ~ServerThreadRecv() = default;
};

#endif
