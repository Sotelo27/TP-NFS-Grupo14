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
    std::cout << "[ServerHandler] Starting recv and send threads..." << std::endl;
    std::cout.flush();
    
    recv.start();
    send.start();
    
    std::cout << "[ServerHandler] Threads started successfully" << std::endl;
    std::cout.flush();
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

void ServerHandler::send_choose_car(uint8_t id_car) {
    ClientMessage msg;
    msg.type = ClientMessage::Type::ChooseCar; 
    msg.car_id = id_car;
    std::cout << "[ServerHandler] Sending choose id car " << (int)id_car << std::endl;
    messages_send.try_push(msg);
}

void ServerHandler::send_start_game(const std::vector<std::pair<std::string, uint8_t>>& races) {
    ClientMessage msg;
    msg.type = ClientMessage::Type::StartGame;
    msg.races = races;
    std::cout << "[ServerHandler] Sending START_GAME with " << races.size() << " race(s)" << std::endl;
    messages_send.try_push(msg);
}

// Overload simple: una sola carrera
void ServerHandler::send_start_game(const std::string& map, uint8_t route) {
    send_start_game({{map, route}});
}

ServerMessage ServerHandler::recv_response_from_server() {
    ServerMessage msg;
    if (messages_recv.try_pop(msg)) {
        std::cout << "[ServerHandler] Received message type: " << (int)msg.type << std::endl;
        return msg;
    }
    msg.type = ServerMessage::Type::Empty;
    return msg;
}

void ServerHandler::send_leave_room() {
    ClientMessage msg;
    msg.type = ClientMessage::Type::Room;
    msg.room_cmd = 0x04;  // ROOM_LEAVE
    msg.room_id = 0;      // ignorado
    std::cout << "[ServerHandler] Sending ROOM_LEAVE" << std::endl;
    messages_send.try_push(msg);
}

void ServerHandler::send_improvement_choice(CarImprovement improvement) {
    ClientMessage msg;
    msg.type = ClientMessage::Type::Improvement;
    msg.improvement = static_cast<uint8_t>(improvement);
    std::cout << "[ServerHandler] Sending Improvement choice: " << static_cast<int>(improvement) << std::endl;
    messages_send.try_push(msg);
}
