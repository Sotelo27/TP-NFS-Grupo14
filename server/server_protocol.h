#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

#include "../common/constants.h"
#include "../common/socket.h"
#include "../common/dto/client_msg.h"
#include "../common/dto/server_msg.h"
#include "../common/dto/room_info.h"
#include "../common/dto/car_info.h"
#include "../common/dto/results_info.h"
#include "../common/dto/map_tick_info.h"
#include "../common/dto/player_info.h"

class ServerProtocol {
private:
    Socket skt;
    std::unordered_map<uint8_t, std::function<ClientMessage()>> recv_dispatch;
    void init_recv_dispatch();
    ClientMessage parse_name();
    ClientMessage parse_move();
    ClientMessage parse_room();
    ClientMessage parse_start_game();
    ClientMessage parse_choose_car();
    ClientMessage parse_improvement();
    ClientMessage parse_cheat();
    ClientMessage parse_exit();

public:
    explicit ServerProtocol(Socket&& skt);

    void send_ok();
    void send_pos(const ServerOutMsg& msg);
    void send_your_id(const ServerOutMsg& msg); 
    void send_player_name(const ServerOutMsg& msg);
    void send_rooms(const std::vector<RoomInfo>& rooms);
    void send_room_created(uint8_t room_id);
    void send_players_list(const std::vector<PlayerInfo>& players);
    ClientMessage receive();
    void enviar_mensaje(uint16_t cantidad_nitros_activos, uint8_t mensaje);
    void enviar_rooms_default();
    void send_cars_list(const std::vector<CarInfo>& cars);
    void send_race_start(uint8_t map_id, uint8_t amount_checkpoints,
                         const std::vector<std::pair<int32_t,int32_t>>& checkpoints);
    void send_results(const std::vector<PlayerResultCurrent>& current,
                      const std::vector<PlayerResultTotal>& total);
    void send_map_info(const std::vector<PlayerTickInfo>& players,
                       const std::vector<NpcTickInfo>& npcs,
                       const std::vector<EventInfo>& events,
                       TimeTickInfo time_info);

    bool is_recv_closed() const { return skt.is_stream_recv_closed(); }
    void shutdown(int mode) { skt.shutdown(mode); }

    ServerProtocol(const ServerProtocol&) = delete;
    ServerProtocol& operator=(const ServerProtocol&) = delete;
    ServerProtocol(ServerProtocol&&) = default;
    ServerProtocol& operator=(ServerProtocol&&) = default;
    ~ServerProtocol() = default;
};

#endif
