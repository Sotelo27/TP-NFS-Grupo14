#include "server_handler.h"

#include <iostream>
#include <utility>

#include "../../common/constants.h"

ServerHandler::ServerHandler(Socket&& skt, Queue<ServerMessage>& server_actions):
        protocol(ClientProtocol(std::move(skt))),
        messages_send{},
        recv(protocol, server_actions),
        send(protocol, messages_send) {}

ServerHandler::~ServerHandler() {
    hard_kill();
    messages_send.close();
    recv.join();
    send.join();
}

void ServerHandler::start() {
    recv.start();
    send.start();
}

bool ServerHandler::is_alive() {
    if (!recv.is_alive() || !send.is_alive()) {
        return false;
    }

    return true;
}

void ServerHandler::hard_kill() {
    recv.stop();
    send.stop();
    if (!protocol.is_recv_closed()) {
        protocol.shutdown(SHUT_BOTH_CLOSED);
    }
}

void ServerHandler::send_movement(Movement mov) { messages_send.push(client_msg_pos{mov}); }
