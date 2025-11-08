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
    if (len > 0) {
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
    buf.reserve(2);
    buf.push_back(code);
    buf.push_back(qty);

    for (const auto& r : races) {
        const std::string& map = r.first;
        uint8_t route = r.second;
        uint16_t len = (uint16_t)map.size();
        uint16_t len_be = htons(len);

        size_t old = buf.size();
        buf.resize(old + 2);
        std::memcpy(buf.data() + old, &len_be, 2);
        if (len > 0) {
            buf.insert(buf.end(), map.begin(), map.end());
        }
        buf.push_back(route);
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

ServerMessage ClientProtocol::receive() {
    ServerMessage dto;
    dto.type = ServerMessage::Type::Unknown;

    uint8_t code = 0;
    int r = skt.recvall(&code, sizeof(code));
    if (r == 0) {
        return dto;
    }

    if (code == CODE_S2C_OK) {
        dto.type = ServerMessage::Type::Ok;
    } else if (code == CODE_S2C_POS) {
        dto.type = ServerMessage::Type::Pos;

        uint32_t id_be = 0;
        uint16_t x_be = 0, y_be = 0;
        uint32_t ang_be = 0;

        skt.recvall(&id_be, sizeof(id_be));
        skt.recvall(&x_be, sizeof(x_be));
        skt.recvall(&y_be, sizeof(y_be));
        skt.recvall(&ang_be, sizeof(ang_be));

        dto.id = ntohl(id_be);
        dto.x = ntohs(x_be);
        dto.y = ntohs(y_be);
        dto.angle = ntohf32(ang_be);
    } else if (code == CODE_S2C_ROOMS) {
        dto.type = ServerMessage::Type::Rooms;
        uint8_t count = 0;
        skt.recvall(&count, sizeof(count));
        dto.rooms.clear();
        dto.rooms.reserve(count);
        for (uint8_t i = 0; i < count; ++i) {
            RoomInfo rinfo{};
            skt.recvall(&rinfo.id, sizeof(rinfo.id));
            skt.recvall(&rinfo.current_players, sizeof(rinfo.current_players));
            skt.recvall(&rinfo.max_players, sizeof(rinfo.max_players));
            dto.rooms.push_back(rinfo);
        }
    } else if (code == CODE_S2C_PLAYER_NAME) {
        uint32_t id_be = 0;
        uint16_t len_be = 0;
        skt.recvall(&id_be, sizeof(id_be));
        skt.recvall(&len_be, sizeof(len_be));
        uint16_t len = ntohs(len_be);
        std::string name(len, '\0');
        if (len > 0) {
            skt.recvall(&name[0], len);
        }
        dto.type = ServerMessage::Type::PlayerName;
        dto.id = ntohl(id_be);
        dto.username = name;
    } else if (code == CODE_S2C_YOUR_ID) {
        uint32_t id_be = 0;
        skt.recvall(&id_be, sizeof(id_be));
        dto.id = ntohl(id_be);
        dto.type = ServerMessage::Type::YourId;
    } else if (code == CODE_S2C_ROOM_CREATED) {
        uint8_t room_id = 0;
        skt.recvall(&room_id, sizeof(room_id));
        dto.type = ServerMessage::Type::RoomCreated;
        dto.id = room_id;
    } else if (code == CODE_S2C_PLAYERS_LIST) {
        dto.type = ServerMessage::Type::PlayersList;
        uint8_t count = 0;
        skt.recvall(&count, sizeof(count));
        
        dto.players.clear();
        dto.players.reserve(count);
        
        for (uint8_t i = 0; i < count; ++i) {
            PlayerInfo pinfo;
            
            // Player ID (4 bytes big endian)
            uint32_t id_be = 0;
            skt.recvall(&id_be, sizeof(id_be));
            pinfo.player_id = ntohl(id_be);
            
            // Username length (2 bytes big endian)
            uint16_t len_be = 0;
            skt.recvall(&len_be, sizeof(len_be));
            uint16_t len = ntohs(len_be);
            
            // Username string
            pinfo.username.resize(len);
            if (len > 0) {
                skt.recvall(&pinfo.username[0], len);
            }
            
            // Ready flag (1 byte)
            uint8_t ready = 0;
            skt.recvall(&ready, sizeof(ready));
            pinfo.is_ready = (ready != 0);
            
            // Health (1 byte)
            uint8_t health = 0;
            skt.recvall(&health, sizeof(health));
            pinfo.health = health;
            
            // Race time (4 bytes big endian)
            uint32_t time_be = 0;
            skt.recvall(&time_be, sizeof(time_be));
            pinfo.race_time_ms = ntohl(time_be);
            
            dto.players.push_back(pinfo);
            
            std::cout << "[ClientProtocol] Player " << pinfo.player_id 
                      << " '" << pinfo.username << "' health=" << (int)pinfo.health
                      << " time=" << pinfo.race_time_ms << "ms\n";
        }
        
        std::cout << "[ClientProtocol] Received PLAYERS_LIST with " << (int)count << " players\n";
    } else if (code == CODE_S2C_GAME_OVER) {
        dto.type = ServerMessage::Type::GameOver;
    } else if (code == CODE_S2C_CAR_LIST) {
        uint8_t count = 0;
        skt.recvall(&count, sizeof(count));
        for (uint8_t i = 0; i < count; ++i) {
            uint8_t tmp[6];
            skt.recvall(tmp, sizeof(tmp));
        }
    } else if (code == CODE_S2C_RACE_START) {
        uint16_t len_be = 0; skt.recvall(&len_be, sizeof(len_be));
        uint16_t len = ntohs(len_be);
        std::string map(len, '\0');
        if (len) skt.recvall(&map[0], len);
        uint8_t amount = 0; skt.recvall(&amount, sizeof(amount));
        for (uint8_t i = 0; i < amount; ++i) {
            int32_t x=0,y=0;
            skt.recvall(&x, sizeof(x));
            skt.recvall(&y, sizeof(y));
        }
    } else if (code == CODE_S2C_RESULTS) {
        uint8_t nplayers = 0; skt.recvall(&nplayers, sizeof(nplayers));
        for (uint8_t i=0;i<nplayers;++i){
            uint16_t lbe=0; skt.recvall(&lbe,2);
            uint16_t l=ntohs(lbe);
            std::string name(l, '\0'); if(l) skt.recvall(&name[0], l);
            uint16_t time_be=0; skt.recvall(&time_be,2);
        }
        for (uint8_t i=0;i<nplayers;++i){
            uint16_t lbe=0; skt.recvall(&lbe,2);
            uint16_t l=ntohs(lbe);
            std::string name(l, '\0'); if(l) skt.recvall(&name[0], l);
            uint32_t tbe=0; skt.recvall(&tbe,4);
        }
    } else if (code == CODE_S2C_MAP_INFO) {
        uint8_t np=0; skt.recvall(&np,1);
        for(uint8_t i=0;i<np;++i){
            uint16_t lbe=0; skt.recvall(&lbe,2);
            uint16_t l=ntohs(lbe);
            std::string user(l, '\0'); if(l) skt.recvall(&user[0], l);
            uint8_t car=0; skt.recvall(&car,1);
            int32_t px=0,py=0; skt.recvall(&px,4); skt.recvall(&py,4);
        }
        uint8_t nn=0; skt.recvall(&nn,1);
        for(uint8_t i=0;i<nn;++i){
            uint8_t npcid=0; skt.recvall(&npcid,1);
            int32_t px=0,py=0; skt.recvall(&px,4); skt.recvall(&py,4);
        }
        uint8_t ne=0; skt.recvall(&ne,1);
        for(uint8_t i=0;i<ne;++i){
            uint8_t et=0; skt.recvall(&et,1);
            uint16_t lbe=0; skt.recvall(&lbe,2);
            uint16_t l=ntohs(lbe);
            std::string user(l, '\0'); if(l) skt.recvall(&user[0], l);
        }
    }

    return dto;
}