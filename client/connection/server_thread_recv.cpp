#include "server_thread_recv.h"
#include <iostream>

ServerThreadRecv::ServerThreadRecv(ClientProtocol& protocol, Queue<ServerMessage>& server_actions):
        protocol(protocol), server_actions(server_actions) {}

void ServerThreadRecv::run() {
    std::cout << "[ServerThreadRecv] Thread started" << std::endl;
    std::cout.flush();
    
    while (should_keep_running()) {
        try {
            std::cout << "[ServerThreadRecv] Calling protocol.receive()..." << std::endl;
            std::cout.flush();
            
            ServerMessage msg = protocol.receive();

            std::cout << "[ServerThreadRecv] Received message type=" << (int)msg.type << std::endl;
            std::cout.flush();
            
            server_actions.push(msg);
            std::cout << "[ServerThreadRecv] Pushed message to queue" << std::endl;
            std::cout.flush();

            if (protocol.is_recv_closed()) {
                std::cout << "[ServerThreadRecv] Connection closed by server" << std::endl;
                std::cout.flush();
                break;
            }
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "[ServerThreadRecv] Error receiving message from server: " << e.what() << "\n";
            std::cerr.flush();
            break;
        }
    }
    
    std::cout << "[ServerThreadRecv] Thread stopped" << std::endl;
    std::cout.flush();
}
