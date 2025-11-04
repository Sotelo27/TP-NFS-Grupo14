#include "client_thread_send.h"
#include <cmath>

#include "client_action.h"
#define  PI 3.14159265358979323846f

ClientThreadSend::ClientThreadSend(ServerProtocol& protocol, size_t id,
                                 Queue<ClientMessage>& mensajes_a_enviar):
    protocol(protocol), id(id), mensajes_a_enviar(mensajes_a_enviar) {}

void ClientThreadSend::run() {
    while (should_keep_running()) {
        try {
            ClientMessage msg = mensajes_a_enviar.pop();
            if (msg.type == ClientMessage::Type::Move) {
                float angle_deg = msg.angle * 180.0f / PI;
                protocol.send_pos(id, msg.x, msg.y, angle_deg);
            } else if (msg.type == ClientMessage::Type::Room) {}

        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error sending message to client " << id << ": " << e.what() << "\n";
            break;
        }
    }
}
