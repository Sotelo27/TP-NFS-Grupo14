#include "client_thread_send.h"
#include <cmath>

#include "client_action.h"
#define  PI 3.14159265358979323846f

ClientThreadSend::ClientThreadSend(ServerProtocol& protocol, size_t id,
                                 Queue<ServerOutMsg>& mensajes_a_enviar):
    protocol(protocol), id(id), mensajes_a_enviar(mensajes_a_enviar) {}

void ClientThreadSend::run() {
    while (should_keep_running()) {
        try {
            ServerOutMsg msg = mensajes_a_enviar.pop();
            switch (msg.type) {
                case ServerOutType::Ok:
                    protocol.send_ok();
                    break;
                case ServerOutType::Pos: {
                    float angle_deg = msg.angle * 180.0f / PI;
                    protocol.send_pos(msg.id, msg.x, msg.y, angle_deg);
                } break;
                case ServerOutType::YourId:
                    protocol.send_your_id(msg.your_id);
                    break;
                case ServerOutType::PlayerName:
                    protocol.send_player_name(msg.id, msg.username);
                    break;
                case ServerOutType::Rooms:
                    protocol.send_rooms(msg.rooms);
                    break;
                case ServerOutType::RoomCreated:
                    protocol.send_room_created(msg.room_id);
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
                    break;
            }
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }
            std::cerr << "Error sending message to client " << id << ": " << e.what() << "\n";
            break;
        }
    }
}

