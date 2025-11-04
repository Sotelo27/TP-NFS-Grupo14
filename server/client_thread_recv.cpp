#include "client_thread_recv.h"

#include <utility>

ClientThreadRecv::ClientThreadRecv(ServerProtocol& protocol, size_t id,
                 Queue<ClientAction>& actiones_clients):
    protocol(protocol), id(id), actiones_clients(actiones_clients) {}

void ClientThreadRecv::run() {
    std::cout << "[ClientThreadRecv] Thread started for client " << id << "\n";
    std::cout.flush();
    
    while (should_keep_running()) {
        try {
            std::cout << "[ClientThreadRecv] Client " << id << " - Calling protocol.receive()...\n";
            std::cout.flush();
            
            ClientMessage received = protocol.receive();
            
            std::cout << "[ClientThreadRecv] Client " << id << " - protocol.receive() returned\n";
            std::cout.flush();
            
            if (protocol.is_recv_closed()) {
                std::cout << "[ClientThreadRecv] Connection closed by client " << id << "\n";
                std::cout.flush();
                break;
            }

            std::cout << "[ClientThreadRecv] Received message type=" << (int)received.type << " from client " << id << "\n";
            std::cout.flush();

            if (received.type == ClientMessage::Type::Move) {
                ClientAction msg = {id, received.movement};
                actiones_clients.push(msg);
                std::cout << "[ClientThreadRecv] Pushed MOVE action from client " << id << "\n";
                std::cout.flush();
            } else if (received.type == ClientMessage::Type::Name) {
                ClientAction msg = {id, std::move(received.username)};
                actiones_clients.push(msg);
                std::cout << "[ClientThreadRecv] Pushed NAME('" << msg.username << "') action from client " << id << "\n";
                std::cout.flush();
            } else if (received.type == ClientMessage::Type::Room) {
                ClientAction msg;
                msg.type = ClientAction::Type::Room;
                msg.id = id;
                msg.room_cmd = received.room_cmd;
                msg.room_id = received.room_id;
                actiones_clients.push(msg);
                std::cout << "[ClientThreadRecv] Pushed ROOM action from client " << id << " (cmd=" << (int)received.room_cmd << ")\n";
                std::cout.flush();
            } else {
                std::cout << "[ClientThreadRecv] Unhandled message type " << (int)received.type << " from client " << id << "\n";
                std::cout.flush();
            }
        } catch (const ClosedQueue&) {
            std::cout << "[ClientThreadRecv] Queue closed for client " << id << "\n";
            std::cout.flush();
            break;
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "[ClientThreadRecv] Error receiving action from client " << id << ": " << e.what() << "\n";
            std::cerr.flush();
            break;
        }
    }
    
    std::cout << "[ClientThreadRecv] Thread stopped for client " << id << "\n";
    std::cout.flush();
}
