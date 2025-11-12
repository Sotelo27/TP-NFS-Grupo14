#include "client_protocol.h"

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>  
#include <arpa/inet.h>

#include "../common/dto/player_info.h"

#define MOVE_BUF 2

static inline uint32_t htonf32(float f) {
    uint32_t u;
    std::memcpy(&u, &f, sizeof(u));
    return htonl(u);
}

static inline float ntohf32(uint32_t u) {
    u = ntohl(u);
    float f;
    std::memcpy(&f, &u, sizeof(f));
    return f;
}

ClientProtocol::ClientProtocol(Socket&& skt) : skt(std::move(skt)) {
    init_recv_dispatch();
}

void ClientProtocol::init_recv_dispatch() {
    recv_dispatch = {
        {CODE_S2C_OK,           [this](){ return parse_ok(); }},
        {CODE_S2C_POS,          [this](){ return parse_pos(); }},
        {CODE_S2C_ROOMS,        [this](){ return parse_rooms(); }},
        {CODE_S2C_PLAYER_NAME,  [this](){ return parse_player_name(); }},
        {CODE_S2C_YOUR_ID,      [this](){ return parse_your_id(); }},
        {CODE_S2C_ROOM_CREATED, [this](){ return parse_room_created(); }},
        {CODE_S2C_PLAYERS_LIST, [this](){ return parse_players_list(); }},
        {CODE_S2C_GAME_OVER,    [this](){ return parse_game_over(); }},
        {CODE_S2C_CAR_LIST,     [this](){ return parse_car_list(); }},
        {CODE_S2C_RACE_START,   [this](){ return parse_race_start(); }},
        {CODE_S2C_RESULTS,      [this](){ return parse_results(); }},
        {CODE_S2C_MAP_INFO,     [this](){ return parse_map_info(); }}
    };
}

ServerMessage ClientProtocol::parse_ok() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Ok;
    return dto;
}

ServerMessage ClientProtocol::parse_pos() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Pos;
    uint32_t id_be=0;
    uint16_t x_be=0, y_be=0;
    uint32_t ang_be=0;
    skt.recvall(&id_be, 4);
    skt.recvall(&x_be, 2);
    skt.recvall(&y_be, 2);
    skt.recvall(&ang_be, 4);
    dto.id = ntohl(id_be);
    dto.x = (int16_t)ntohs(x_be);
    dto.y = (int16_t)ntohs(y_be);
    dto.angle = ntohf32(ang_be);
    return dto;
}

ServerMessage ClientProtocol::parse_rooms() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Rooms;
    uint8_t count=0;
    skt.recvall(&count, 1);
    dto.rooms.clear();
    dto.rooms.reserve(count);
    for(uint8_t i=0; i<count; ++i) {
        RoomInfo r{};
        skt.recvall(&r.id, 1);
        skt.recvall(&r.current_players, 1);
        skt.recvall(&r.max_players, 1);
        dto.rooms.push_back(r);
    }
    return dto;
}

ServerMessage ClientProtocol::parse_player_name() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::PlayerName;
    uint32_t id_be=0;
    uint16_t len_be=0;
    skt.recvall(&id_be, 4);
    skt.recvall(&len_be, 2);
    uint16_t len = ntohs(len_be);
    std::string name(len, '\0');
    if(len) skt.recvall(&name[0], len);
    dto.id = ntohl(id_be);
    dto.username = std::move(name);
    return dto;
}

ServerMessage ClientProtocol::parse_your_id() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::YourId;
    uint32_t id_be=0;
    skt.recvall(&id_be, 4);
    dto.id = ntohl(id_be);
    return dto;
}

ServerMessage ClientProtocol::parse_room_created() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::RoomCreated;
    uint8_t room_id=0;
    skt.recvall(&room_id, 1);
    dto.id = room_id;
    return dto;
}

ServerMessage ClientProtocol::parse_players_list() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::PlayersList;
    uint8_t count=0;
    skt.recvall(&count, 1);
    dto.players.clear();
    dto.players.reserve(count);
    
    for(uint8_t i=0; i<count; ++i) {
        PlayerInfo p{};
        uint32_t id_be=0;
        skt.recvall(&id_be, 4);
        p.player_id = ntohl(id_be);
        
        uint16_t len_be=0;
        skt.recvall(&len_be, 2);
        uint16_t len = ntohs(len_be);
        p.username.resize(len);
        if(len) skt.recvall(&p.username[0], len);
        
        uint8_t ready=0;
        skt.recvall(&ready, 1);
        p.is_ready = (ready != 0);
        
        uint8_t health=0;
        skt.recvall(&health, 1);
        p.health = health;
        
        uint32_t time_be=0;
        skt.recvall(&time_be, 4);
        p.race_time_ms = ntohl(time_be);
        
        dto.players.push_back(p);
    }
    
    std::cout << "[ClientProtocol] Received PLAYERS_LIST with " << (int)count << " players\n";
    return dto;
}

ServerMessage ClientProtocol::parse_game_over() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::GameOver;
    return dto;
}

ServerMessage ClientProtocol::parse_car_list() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Unknown;
    uint8_t count=0;
    skt.recvall(&count, 1);
    for(uint8_t i=0; i<count; ++i) {
        uint8_t tmp[6];
        skt.recvall(tmp, sizeof(tmp));
    }
    return dto;
}

ServerMessage ClientProtocol::parse_race_start() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::RaceStart;
    
    uint16_t len_be=0;
    skt.recvall(&len_be, 2);
    uint16_t len = ntohs(len_be);
    std::string map_name(len, '\0');
    if(len > 0) {
        skt.recvall(&map_name[0], len);
    }
    
    uint8_t amount=0;
    skt.recvall(&amount, 1);
    
    for(uint8_t i=0; i<amount; ++i) {
        int32_t x_be=0, y_be=0;
        skt.recvall(&x_be, 4);
        skt.recvall(&y_be, 4);
    }
    
    std::cout << "[ClientProtocol] RaceStart recibido: mapa=" << map_name 
              << ", checkpoints=" << (int)amount << std::endl;
    
    return dto;
}

ServerMessage ClientProtocol::parse_results() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Unknown;
    uint8_t n=0;
    skt.recvall(&n, 1);
    
    for(uint8_t i=0; i<n; ++i) {
        uint16_t lbe=0;
        skt.recvall(&lbe, 2);
        uint16_t l = ntohs(lbe);
        std::string name(l, '\0');
        if(l) skt.recvall(&name[0], l);
        uint16_t time_be=0;
        skt.recvall(&time_be, 2);
    }
    
    for(uint8_t i=0; i<n; ++i) {
        uint16_t lbe=0;
        skt.recvall(&lbe, 2);
        uint16_t l = ntohs(lbe);
        std::string name(l, '\0');
        if(l) skt.recvall(&name[0], l);
        uint32_t tbe=0;
        skt.recvall(&tbe, 4);
    }
    return dto;
}

ServerMessage ClientProtocol::parse_map_info() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::MapInfo;
    dto.players_tick.clear();
    dto.npcs_tick.clear();
    dto.events_tick.clear();
    
    uint8_t np=0;
    skt.recvall(&np, 1);
    for(uint8_t i=0; i<np; ++i) {
        uint16_t lbe=0;
        skt.recvall(&lbe, 2);
        uint16_t l = ntohs(lbe);
        std::string user(l, '\0');
        if(l) skt.recvall(&user[0], l);
        
        uint8_t car=0;
        skt.recvall(&car, 1);
        uint32_t pid_be=0;
        skt.recvall(&pid_be, 4);
        uint32_t x_be=0, y_be=0;
        skt.recvall(&x_be, 4);
        skt.recvall(&y_be, 4);
        uint32_t ang_be=0;
        skt.recvall(&ang_be, 4);
        uint8_t health=0;
        skt.recvall(&health, 1);
        
        PlayerTickInfo pti;
        pti.username = std::move(user);
        pti.car_id = car;
        pti.player_id = ntohl(pid_be);
        pti.x = (int32_t)ntohl(x_be);
        pti.y = (int32_t)ntohl(y_be);
        pti.angle = ntohf32(ang_be);
        pti.health = health;
        dto.players_tick.push_back(std::move(pti));
    }
    
    uint8_t nn=0;
    skt.recvall(&nn, 1);
    for(uint8_t i=0; i<nn; ++i) {
        uint8_t npcid=0;
        skt.recvall(&npcid, 1);
        uint32_t x_be=0, y_be=0;
        skt.recvall(&x_be, 4);
        skt.recvall(&y_be, 4);
        NpcTickInfo nti;
        nti.npc_id = npcid;
        nti.x = (int32_t)ntohl(x_be);
        nti.y = (int32_t)ntohl(y_be);
        dto.npcs_tick.push_back(nti);
    }
    
    uint8_t ne=0;
    skt.recvall(&ne, 1);
    for(uint8_t i=0; i<ne; ++i) {
        uint8_t et=0;
        skt.recvall(&et, 1);
        uint16_t lbe=0;
        skt.recvall(&lbe, 2);
        uint16_t l = ntohs(lbe);
        std::string user(l, '\0');
        if(l) skt.recvall(&user[0], l);
        EventInfo ei;
        ei.event_type = et;
        ei.username = std::move(user);
        dto.events_tick.push_back(std::move(ei));
    }
    return dto;
}

ServerMessage ClientProtocol::receive() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Unknown;
    uint8_t code=0;
    int r = skt.recvall(&code, 1);
    if(r == 0) return dto;
    
    auto it = recv_dispatch.find(code);
    if(it != recv_dispatch.end()) {
        return it->second();
    }
    return dto;
}

void ClientProtocol::send_name(const std::string& username) {
    uint8_t code = CODE_C2S_NAME;
    uint16_t len = (uint16_t)username.size();
    uint16_t len_be = htons(len);
    
    std::vector<uint8_t> buf;
    buf.reserve(1 + 2 + username.size());
    buf.push_back(code);
    
    size_t off = buf.size();
    buf.resize(off + 2);
    std::memcpy(buf.data() + off, &len_be, 2);
    
    if(len > 0) {
        buf.insert(buf.end(), username.begin(), username.end());
    }
    
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ClientProtocol::send_move(Movement mov) {
    uint8_t code = CODE_C2S_MOVE;
    uint8_t mv = (uint8_t)mov;
    uint8_t buf[MOVE_BUF] = {code, mv};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_create_room() {
    uint8_t code = CODE_C2S_ROOM;
    uint8_t sub = ROOM_CREATE;
    uint8_t buf[2] = {code, sub};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_join_room(uint8_t room_id) {
    uint8_t code = CODE_C2S_ROOM;
    uint8_t sub = ROOM_JOIN;
    uint8_t buf[3] = {code, sub, room_id};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_start_game(const std::vector<std::pair<std::string, uint8_t>>& races) {
    uint8_t code = CODE_C2S_START_GAME;
    uint8_t qty = (uint8_t)races.size();
    
    std::vector<uint8_t> buf;
    buf.reserve(2 + races.size() * 50);
    buf.push_back(code);
    buf.push_back(qty);
    
    for(const auto& race : races) {
        uint16_t len = (uint16_t)race.first.size();
        uint16_t len_be = htons(len);
        
        size_t off = buf.size();
        buf.resize(off + 2);
        std::memcpy(buf.data() + off, &len_be, 2);
        
        if(len > 0) {
            off = buf.size();
            buf.resize(off + race.first.size());
            std::memcpy(buf.data() + off, race.first.data(), race.first.size());
        }
        
        buf.push_back(race.second);
    }
    
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ClientProtocol::send_choose_car(uint8_t car_id) {
    uint8_t code = CODE_C2S_CHOOSE_CAR;
    uint8_t buf[2] = {code, car_id};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_improvement(uint8_t improvement) {
    uint8_t code = CODE_C2S_IMPROVEMENT;
    uint8_t buf[2] = {code, improvement};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_cheat(uint8_t cheat_code) {
    uint8_t code = CODE_C2S_CHEAT;
    uint8_t buf[2] = {code, cheat_code};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_exit() {
    uint8_t code = CODE_C2S_EXIT;
    skt.sendall(&code, sizeof(code));
}