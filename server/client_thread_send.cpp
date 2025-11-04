#include "client_thread_send.h"
#include <cmath>

#include "client_action.h"
#define  PI 3.14159265358979323846f

ClientThreadSend::ClientThreadSend(ServerProtocol& protocol, size_t id,
                                 Queue<ServerOutMsg>& mensajes_a_enviar):
    protocol(protocol), id(id), mensajes_a_enviar(mensajes_a_enviar) {}

void ClientThreadSend::run() {
    std::cout << "[ClientThreadSend] Thread started for client " << id << "\n";
    std::cout.flush();
    
    while (should_keep_running()) {
        try {
            ServerOutMsg msg = mensajes_a_enviar.pop();
            
            // Solo logear mensajes que NO sean POS para no saturar
            if (msg.type != ServerOutType::Pos) {
                std::cout << "[ClientThreadSend] Sending message type=" << (int)msg.type << " to client " << id << "\n";
                std::cout.flush();
            }
            
            switch (msg.type) {
                case ServerOutType::Ok:
                    protocol.send_ok();
                    std::cout << "[ClientThreadSend] Sent OK to client " << id << "\n";
                    std::cout.flush();
                    break;
                case ServerOutType::Pos: {
                    float angle_deg = msg.angle * 180.0f / PI;
                    protocol.send_pos(msg.id, msg.x, msg.y, angle_deg);
                    // NO logear cada posición para no saturar la consola
                } break;
                case ServerOutType::YourId:
                    protocol.send_your_id(msg.your_id);
                    std::cout << "[ClientThreadSend] Sent YOUR_ID(" << msg.your_id << ") to client " << id << "\n";
                    std::cout.flush();
                    break;
                case ServerOutType::PlayerName:
                    protocol.send_player_name(msg.id, msg.username);
                    std::cout << "[ClientThreadSend] Sent PLAYER_NAME to client " << id << "\n";
                    std::cout.flush();
                    break;
                case ServerOutType::Rooms:
                    protocol.send_rooms(msg.rooms);
                    std::cout << "[ClientThreadSend] Sent ROOMS(" << msg.rooms.size() << ") to client " << id << "\n";
                    std::cout.flush();
                    break;
                case ServerOutType::RoomCreated:
                    protocol.send_room_created(msg.room_id);
                    std::cout << "[ClientThreadSend] Sent ROOM_CREATED(" << (int)msg.room_id << ") to client " << id << "\n";
                    std::cout.flush();
                    break;
                case ServerOutType::CarList:
                    protocol.send_cars_list(msg.cars);
                    break;
                case ServerOutType::RaceStart:
                    protocol.send_race_start(msg.map_name, msg.amount_checkpoints, msg.checkpoints);
                    break;
                case ServerOutType::Results:
                    protocol.send_results(msg.results_current, msg.results_total);
                    break;
                case ServerOutType::MapInfo:
                    protocol.send_map_info(msg.players_tick, msg.npcs_tick, msg.events_tick);
                    break;
                default:
                    std::cout << "[ClientThreadSend] Unknown message type\n";
                    break;
            }
        } catch (const ClosedQueue&) {
            std::cout << "[ClientThreadSend] Queue closed for client " << id << "\n";
            std::cout.flush();
            break;
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }
            std::cerr << "[ClientThreadSend] Error sending message to client " << id << ": " << e.what() << "\n";
            std::cerr.flush();
            break;
        }
    }
    
    std::cout << "[ClientThreadSend] Thread stopped for client " << id << "\n";
    std::cout.flush();
}

