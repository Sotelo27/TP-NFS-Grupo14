#include "server_thread_send.h"

#include "../../common/dto/client_msg.h"
#include <iostream>

ServerThreadSend::ServerThreadSend(ClientProtocol& protocol, Queue<ClientMessage>& messages_send):
        protocol(protocol), messages_send(messages_send) {}

void ServerThreadSend::run() {
    std::cout << "[ServerThreadSend] Thread started" << std::endl;
    std::cout.flush();

    while (should_keep_running()) {
        try {
            std::cout << "[ServerThreadSend] Waiting for message to send..." << std::endl;
            std::cout.flush();
            
            ClientMessage msg = messages_send.pop();

            std::cout << "[ServerThreadSend] Got message type=" << (int)msg.type << ", sending..." << std::endl;
            std::cout.flush();

            if (msg.type == ClientMessage::Type::Name) {
                protocol.send_name(msg.username);
                std::cout << "[ServerThreadSend] Sent NAME: '" << msg.username << "'" << std::endl;
                std::cout.flush();
            } else if (msg.type == ClientMessage::Type::Move) {
                protocol.send_move(msg.movement);
                std::cout << "[ServerThreadSend] Sent MOVE" << std::endl;
                std::cout.flush();
            } else if (msg.type == ClientMessage::Type::Room) {
                if (msg.room_cmd == ROOM_CREATE) {
                    protocol.send_create_room();
                    std::cout << "[ServerThreadSend] Sent ROOM_CREATE" << std::endl;
                    std::cout.flush();
                } else if (msg.room_cmd == ROOM_JOIN) {
                    protocol.send_join_room(msg.room_id);
                    std::cout << "[ServerThreadSend] Sent ROOM_JOIN(" << (int)msg.room_id << ")" << std::endl;
                    std::cout.flush();
                }
            } else if (msg.type == ClientMessage::Type::ChooseCar) {
                protocol.send_choose_car(msg.car_id);
                std::cout << "[ServerThreadSend] Sent CHOOSE CAR" << std::endl;
                std::cout.flush();
            }

        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "[ServerThreadSend] Error sending message to server: " << e.what() << "\n";
            std::cerr.flush();
            break;
        }
    }

    std::cout << "[ServerThreadSend] Thread stopped" << std::endl;
    std::cout.flush();
}
