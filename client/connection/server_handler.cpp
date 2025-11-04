#include "server_handler.h"

#include <iostream>
#include <utility>

#include "../../common/constants.h"
#include "../../common/dto/client_msg.h"

ServerHandler::ServerHandler(Socket&& skt):
        protocol(ClientProtocol(std::move(skt))),
        messages_send(), messages_recv(),
        recv(protocol, messages_recv),
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

void ServerHandler::send_movement(Movement mov) {
    ClientMessage msg;
    msg.type = ClientMessage::Type::Move;
    msg.movement = mov;
    messages_send.try_push(msg);
}

void ServerHandler::send_username(const std::string& username) {
    ClientMessage msg;
    msg.type = ClientMessage::Type::Name;
    msg.username = username;
    std::cout << "[ServerHandler] Sending username: " << username << std::endl;
    messages_send.try_push(msg);
}

void ServerHandler::send_create_room() {
    ClientMessage msg;
    msg.type = ClientMessage::Type::Room;
    msg.room_cmd = ROOM_CREATE;
    msg.room_id = 0;
    std::cout << "[ServerHandler] Sending ROOM_CREATE" << std::endl;
    messages_send.try_push(msg);
}

void ServerHandler::send_join_room(uint8_t room_id) {
    ClientMessage msg;
    msg.type = ClientMessage::Type::Room;
    msg.room_cmd = ROOM_JOIN;
    msg.room_id = room_id;
    std::cout << "[ServerHandler] Sending ROOM_JOIN to room " << (int)room_id << std::endl;
    messages_send.try_push(msg);
}

ServerMessage ServerHandler::recv_response_from_server() {
    ServerMessage msg;
    if (messages_recv.try_pop(msg)) {
        std::cout << "[ServerHandler] Received message type: " << (int)msg.type << std::endl;
        return msg;
    }
    msg.type = ServerMessage::Type::Unknown;
    return msg;
}