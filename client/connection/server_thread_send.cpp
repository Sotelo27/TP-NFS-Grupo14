#include "server_thread_send.h"

#include "../../common/dto/client_msg.h"

ServerThreadSend::ServerThreadSend(ClientProtocol& protocol, Queue<ClientMessage>& messages_send):
        protocol(protocol), messages_send(messages_send) {}

void ServerThreadSend::run() {
    while (should_keep_running()) {
        try {
            ClientMessage msg = messages_send.pop();
            
            if (msg.type == ClientMessage::Type::Name) {
                protocol.send_name(msg.username);
            } else if (msg.type == ClientMessage::Type::Move) {
                protocol.send_move(msg.movement);
            } else if (msg.type == ClientMessage::Type::Room) {
                protocol.send_create_room();
            }
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error sending message to server: " << e.what() << "\n";
            break;
        }
    }
}
