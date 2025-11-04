#include "server_thread_recv.h"

#include <utility>

ServerThreadRecv::ServerThreadRecv(ClientProtocol& protocol, Queue<ServerMessage>& server_actions):
        protocol(protocol), server_actions(server_actions) {}

void ServerThreadRecv::run() {
    while (should_keep_running()) {
        try {
            ServerMessage msg = protocol.receive();
            if (protocol.is_recv_closed()) {
                break;
            }

            if (msg.type != ServerMessage::Type::Unknown) {
                server_actions.push(msg);
            }
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error receiving message from server: " << e.what() << "\n";
            break;
        }
    }
}
