#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <unordered_map>

#include "../common/constants.h"
#include "../common/socket.h"
#include "../common/dto/movement.h"
#include "../common/dto/server_msg.h"
#include "../common/dto/player_info.h" 
#include "../common/dto/client_msg.h" 

class ClientProtocol {
private:
    Socket skt;
    std::unordered_map<uint8_t, std::function<ServerMessage()>> recv_dispatch;
    void init_recv_dispatch();
    ServerMessage parse_ok();
    ServerMessage parse_pos();
    ServerMessage parse_rooms();
    ServerMessage parse_player_name();
    ServerMessage parse_your_id();
    ServerMessage parse_room_created();
    ServerMessage parse_players_list();
    ServerMessage parse_game_over();
    ServerMessage parse_car_list();     
    ServerMessage parse_race_start();   
    ServerMessage parse_results();      
    ServerMessage parse_map_info();

public:
    explicit ClientProtocol(Socket&& skt);

    // Send username (string)
    void send_name(const ClientMessage& msg);

    // Send movement (enum Movement)
    void send_move(Movement mov);

    // Rooms: crear / unirse / salir
    void send_create_room();
    void send_join_room(const ClientMessage& msg); 
    void send_leave_room();  

    // Start game: cantidad de carreras y (map, route) por carrera
    void send_start_game(const ClientMessage& msg);

    // Elegir auto
    void send_choose_car(const ClientMessage& msg);

    // Mejoras de auto
    void send_improvement(const ClientMessage& msg);

    // Cheats
    void send_cheat(const ClientMessage& msg);

    // Exit match
    void send_exit();

    // Receive() returns ServerMessage DTO
    ServerMessage receive();

    bool is_recv_closed() const { return skt.is_stream_recv_closed(); }
    void shutdown(int mode) { skt.shutdown(mode); }

    ClientProtocol(const ClientProtocol&) = delete;
    ClientProtocol& operator=(const ClientProtocol&) = delete;

    ClientProtocol(ClientProtocol&&) = default;
    ClientProtocol& operator=(ClientProtocol&&) = default;

    ~ClientProtocol() = default;
};

#endif
