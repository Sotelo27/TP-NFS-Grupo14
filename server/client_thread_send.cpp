#include "client_thread_send.h"
#include <cmath>
#include <iostream>  // AGREGAR ESTE INCLUDE

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
            ServerOutMsg msg;
            bool got_msg = mensajes_a_enviar.try_pop(msg);
            if (!got_msg) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            switch (msg.type) {
                case ServerOutType::Pos: {
                    float angle_deg = msg.angle * 180.0f / PI;
                    protocol.send_pos(msg.id, msg.x, msg.y, angle_deg);
                } break;
                case ServerOutType::Rooms:
                    std::cout << "[ClientThreadSend] Sending ROOMS to conn_id=" << id << "\n";
                    protocol.send_rooms(msg.rooms);
                    break;
                case ServerOutType::Ok:
                    std::cout << "[ClientThreadSend] Sending OK to conn_id=" << id << "\n";
                    protocol.send_ok();
                    break;
                case ServerOutType::YourId:
                    std::cout << "[ClientThreadSend] Sending YOUR_ID(" << msg.your_id << ") to conn_id=" << id << "\n";
                    protocol.send_your_id(msg.your_id);
                    break;
                case ServerOutType::PlayerName:
                    std::cout << "[ClientThreadSend] Sending PLAYER_NAME to conn_id=" << id << "\n";
                    protocol.send_player_name(msg.id, msg.username);
                    break;
                case ServerOutType::RoomCreated:
                    std::cout << "[ClientThreadSend] Sending ROOM_CREATED(" << (int)msg.room_id << ") to conn_id=" << id << "\n";
                    protocol.send_room_created(msg.room_id);
                    break;
                case ServerOutType::PlayersList:
                    std::cout << "[ClientThreadSend] Sending PLAYERS_LIST to conn_id=" << id 
                              << " (count=" << msg.players.size() << ")\n";
                    protocol.send_players_list(msg.players);
                    break;
                case ServerOutType::CarList:
                    protocol.send_cars_list(msg.cars);
                    break;
                case ServerOutType::RaceStart:
                    protocol.send_race_start(msg.map_name, (uint8_t)msg.checkpoints.size(), msg.checkpoints);
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

