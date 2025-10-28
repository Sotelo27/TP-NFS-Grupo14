#include "server_thread_send.h"

ServerThreadSend::ServerThreadSend(ClientProtocol& protocol, Queue<client_msg_pos>& messages_send):
        protocol(protocol), messages_send(messages_send) {}

void ServerThreadSend::run() {
    while (should_keep_running()) {
        try {
            client_msg_pos msg = messages_send.pop();
            protocol.send_move(msg.tecla);
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error sending message to server: " << e.what() << "\n";
            break;
        }
    }
}
