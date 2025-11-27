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

            ClientAction action;

            switch (received.type) {
                case ClientMessage::Type::Move: {
                    action = ClientAction(id, received.movement);
                    std::cout << "[ClientThreadRecv] Pushed MOVE action from client " << id << "\n";
                    break;
                }
                case ClientMessage::Type::Name: {
                    action = ClientAction(id, std::move(received.username));
                    std::cout << "[ClientThreadRecv] Pushed NAME('" << action.username << "') action from client " << id << "\n";
                    break;
                }
                case ClientMessage::Type::Room: {
                    action.type = ClientAction::Type::Room;
                    action.id = id;
                    action.room_cmd = received.room_cmd;
                    action.room_id = received.room_id;
                    std::cout << "[ClientThreadRecv] Pushed ROOM action from client " << id << " (cmd=" << (int)received.room_cmd << ")\n";
                    break;
                }
                case ClientMessage::Type::StartGame: {
                    std::cout << "[ClientThreadRecv] START_GAME from conn_id=" << id
                              << " races=" << received.races.size() << "\n";
                    action = ClientAction(id, received.races);
                    actiones_clients.push(action);
                    break;
                }
                case ClientMessage::Type::ChooseCar: {
                    action.type = ClientAction::Type::ChooseCar;
                    action.id = id;
                    action.car_id = received.car_id;
                    std::cout << "[ClientThreadRecv] Pushed CHOOSE_CAR action from client " << id << " (car_id=" << (int)received.car_id << ")\n";
                    break;
                }
                case ClientMessage::Type::Improvement: {
                    //action = ClientAction(id, received.improvement);
                    action.type = ClientAction::Type::Improvement;
                    action.id = id;
                    action.improvement_id = received.improvement;
                    std::cout << "[ClientThreadRecv] Pushed IMPROVEMENT action from client " << id << " (improvement=" << (int)received.improvement << ")\n";
                    break;
                }
                default:
                    std::cout << "[ClientThreadRecv] Unhandled message type " << (int)received.type << " from client " << id << "\n";
                    continue;
            }
            
            actiones_clients.push(action);
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
