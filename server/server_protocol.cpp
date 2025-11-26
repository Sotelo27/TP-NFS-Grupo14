#include "server_protocol.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>  

#include <arpa/inet.h>

void ServerProtocol::send_ok() {
    uint8_t code = CODE_S2C_OK;
    skt.sendall(&code, sizeof(code));
}

static inline uint32_t htonf32(float f) {
    uint32_t u;
    std::memcpy(&u, &f, sizeof(u));
    return htonl(u);
}

void ServerProtocol::send_pos(const ServerOutMsg& msg) {
    uint8_t code = CODE_S2C_POS;

    uint32_t id_be = htonl(msg.id);
    uint16_t x_be = htons((uint16_t)msg.x);
    uint16_t y_be = htons((uint16_t)msg.y);
    uint32_t ang_be = htonf32(msg.angle);

    std::vector<uint8_t> buf;
    buf.reserve(1 + 4 + 2 + 2 + 4);
    buf.push_back(code);

    size_t offset = buf.size();
    buf.resize(offset + 4);
    std::memcpy(buf.data() + offset, &id_be, 4);

    offset = buf.size();
    buf.resize(offset + 2);
    std::memcpy(buf.data() + offset, &x_be, 2);

    offset = buf.size();
    buf.resize(offset + 2);
    std::memcpy(buf.data() + offset, &y_be, 2);

    offset = buf.size();
    buf.resize(offset + 4);
    std::memcpy(buf.data() + offset, &ang_be, 4);

    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_your_id(const ServerOutMsg& msg) {
    uint8_t code = CODE_S2C_YOUR_ID;
    uint32_t id_be = htonl(msg.your_id);

    uint8_t buf[1 + 4];
    buf[0] = code;
    std::memcpy(buf + 1, &id_be, 4);
    skt.sendall(buf, sizeof(buf));
}

void ServerProtocol::send_player_name(const ServerOutMsg& msg) {
    uint8_t code = CODE_S2C_PLAYER_NAME;
    uint32_t id_be = htonl(msg.id);
    uint16_t len = (uint16_t)msg.username.size();
    uint16_t len_be = htons(len);

    std::vector<uint8_t> buf;
    buf.reserve(1 + 4 + 2 + len);
    buf.push_back(code);

    size_t off = buf.size();
    buf.resize(off + 4); std::memcpy(buf.data() + off, &id_be, 4);
    off = buf.size();
    buf.resize(off + 2); std::memcpy(buf.data() + off, &len_be, 2);
    if (len > 0) {
        off = buf.size();
        buf.resize(off + len);
        std::memcpy(buf.data() + off, msg.username.data(), len);
    }

    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_rooms(const std::vector<RoomInfo>& rooms) {
    uint8_t code = CODE_S2C_ROOMS;
    uint8_t count = (uint8_t)rooms.size();

    std::vector<uint8_t> buf;
    buf.reserve(1 + 1 + rooms.size() * 3);
    buf.push_back(code);
    buf.push_back(count);
    for (const auto& r : rooms) {
        buf.push_back(r.id);
        buf.push_back(r.current_players);
        buf.push_back(r.max_players);
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_room_created(const ServerOutMsg& msg) {
    uint8_t code = CODE_S2C_ROOM_CREATED;
    uint8_t buf[2] = {code, msg.room_id};
    skt.sendall(buf, sizeof(buf));
}

void ServerProtocol::send_players_list(const std::vector<PlayerInfo>& players) {
    uint8_t code = CODE_S2C_PLAYERS_LIST;
    uint8_t count = (uint8_t)players.size();
    
    std::vector<uint8_t> buf;
    buf.reserve(2 + players.size() * 30);
    buf.push_back(code);
    buf.push_back(count);
    
    for (const auto& p : players) {
        // Player ID (4 bytes big endian)
        uint32_t id_be = htonl(p.player_id);
        size_t off = buf.size();
        buf.resize(off + 4);
        std::memcpy(buf.data() + off, &id_be, 4);
        
        // Username length (2 bytes big endian)
        uint16_t len = (uint16_t)p.username.size();
        uint16_t len_be = htons(len);
        off = buf.size();
        buf.resize(off + 2);
        std::memcpy(buf.data() + off, &len_be, 2);
        
        // Username string
        if (len > 0) {
            off = buf.size();
            buf.resize(off + p.username.size());
            std::memcpy(buf.data() + off, p.username.data(), p.username.size());
        }
        
        // Ready flag (1 byte)
        buf.push_back(p.is_ready ? 1 : 0);
        
        // NUEVO: Admin flag (1 byte)
        buf.push_back(p.is_admin ? 1 : 0);
        
        // Health (1 byte)
        buf.push_back(p.health);
        
        // Race time (4 bytes big endian)
        uint32_t time_be = htonl(p.race_time_ms);
        off = buf.size();
        buf.resize(off + 4);
        std::memcpy(buf.data() + off, &time_be, 4);
    }
    
    skt.sendall(buf.data(), (unsigned int)buf.size());
    std::cout << "[ServerProtocol] Sent PLAYERS_LIST with " << (int)count << " players\n";
}

void ServerProtocol::send_nitro(const ServerOutMsg& msg) {
    uint8_t code = CODE_SERVER_MSG;
    uint16_t cant = htons(msg.active_nitros); 
    uint8_t msg_type = msg.nitro_msg;                  

    std::vector<uint8_t> paquete(sizeof(code) + sizeof(cant) + sizeof(msg_type));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &code, sizeof(code));
    offset += sizeof(code);
    std::memcpy(paquete.data() + offset, &cant, sizeof(cant));
    offset += sizeof(cant);
    std::memcpy(paquete.data() + offset, &msg_type, sizeof(msg_type));

    skt.sendall(paquete.data(), (unsigned int)paquete.size());
}

void ServerProtocol::send_rooms_default() {
    uint8_t code = CODE_SERVER_MSG;
    uint16_t cant = htons((uint16_t)0);
    uint8_t msg_type = (uint8_t)ERROR_MESSAGE;

    std::vector<uint8_t> paquete(sizeof(code) + sizeof(cant) + sizeof(msg_type));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &code, sizeof(code));
    offset += sizeof(code);
    std::memcpy(paquete.data() + offset, &cant, sizeof(cant));
    offset += sizeof(cant);
    std::memcpy(paquete.data() + offset, &msg_type, sizeof(msg_type));

    skt.sendall(paquete.data(), (unsigned int)paquete.size());
}

void ServerProtocol::send_cars_list(const std::vector<CarInfo>& cars) {
    uint8_t code = CODE_S2C_CAR_LIST;
    uint8_t count = (uint8_t)cars.size();
    std::vector<uint8_t> buf;
    buf.reserve(2 + cars.size()*6);
    buf.push_back(code);
    buf.push_back(count);
    for (const auto& c : cars) {
        buf.push_back(c.id);
        buf.push_back(c.speed);
        buf.push_back(c.acceleration);
        buf.push_back(c.health);
        buf.push_back(c.mass);
        buf.push_back(c.controllability);
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_race_start(uint8_t map_id, uint8_t amount_checkpoints,
                                     const std::vector<std::pair<int32_t,int32_t>>& checkpoints,
                                     uint32_t tiempo_partida) {
    uint8_t code = CODE_S2C_RACE_START;
    std::vector<uint8_t> buf;
    buf.reserve(1 + 1 + 1 + 4 + checkpoints.size()*8);
    buf.push_back(code);
    buf.push_back(map_id); // Enviar el id del mapa como byte
    buf.push_back(amount_checkpoints);
    // NUEVO: enviar tiempo_partida (4 bytes big endian)
    uint32_t tiempo_be = htonl(tiempo_partida);
    size_t off = buf.size();
    buf.resize(off + 4); std::memcpy(buf.data()+off, &tiempo_be, 4);
    for (const auto& cp : checkpoints) {
        int32_t x_be = htonl(cp.first);
        int32_t y_be = htonl(cp.second);
        off = buf.size();
        buf.resize(off + 4); std::memcpy(buf.data()+off, &x_be, 4);
        off = buf.size();
        buf.resize(off + 4); std::memcpy(buf.data()+off, &y_be, 4);
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_results(const std::vector<PlayerResultCurrent>& current,
                                  const std::vector<PlayerResultTotal>& total) {
    uint8_t code = CODE_S2C_RESULTS;
    uint8_t nplayers = (uint8_t)current.size(); // se asume mismo tamaño en ambas
    std::vector<uint8_t> buf;
    buf.reserve(2);
    buf.push_back(code);
    buf.push_back(nplayers);
    // CURRENT
    for (const auto& p : current) {
        // player_id
        uint32_t pid_be = htonl(p.player_id);
        size_t off = buf.size();
        buf.resize(off + 4); std::memcpy(buf.data()+off, &pid_be, 4);

        // username
        uint16_t l = (uint16_t)p.username.size();
        uint16_t lbe = htons(l);
        off = buf.size();
        buf.resize(off + 2); std::memcpy(buf.data()+off, &lbe, 2);
        if (l) {
            off = buf.size();
            buf.resize(off + p.username.size());
            std::memcpy(buf.data()+off, p.username.data(), p.username.size());
        }

        // race_time_seconds (uint32)
        uint32_t race_be = htonl(p.race_time_seconds);
        off = buf.size();
        buf.resize(off + 4); std::memcpy(buf.data()+off, &race_be, 4);

        // total_time_seconds (uint32)
        uint32_t total_be = htonl(p.total_time_seconds);
        off = buf.size();
        buf.resize(off + 4); std::memcpy(buf.data()+off, &total_be, 4);

        // position (uint8)
        buf.push_back(p.position);
    }
    // TOTAL
    for (const auto& p : total) {
        uint16_t l = (uint16_t)p.username.size();
        uint16_t lbe = htons(l);
        size_t off = buf.size();
        buf.resize(off + 2); std::memcpy(buf.data()+off, &lbe, 2);
        if (l) {
            off = buf.size();
            buf.resize(off + p.username.size());
            std::memcpy(buf.data()+off, p.username.data(), p.username.size());
        }
        uint32_t tbe = htonl(p.total_time_seconds);
        off = buf.size();
        buf.resize(off + 4); std::memcpy(buf.data()+off, &tbe, 4);
        buf.push_back(p.position);
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_map_info(const std::vector<PlayerTickInfo>& players,
                                   const std::vector<NpcTickInfo>& npcs,
                                   const std::vector<EventInfo>& events,
                                   TimeTickInfo time_info) {
    uint8_t code = CODE_S2C_MAP_INFO;
    std::vector<uint8_t> buf;
    buf.reserve(1);
    buf.push_back(code);
    // players
    buf.push_back((uint8_t)players.size());
    for (const auto& p : players) {
        uint16_t l = (uint16_t)p.username.size();
        uint16_t lbe = htons(l);
        size_t off = buf.size();
        buf.resize(off + 2); std::memcpy(buf.data()+off, &lbe, 2);
        if (l) {
            off = buf.size();
            buf.resize(off + p.username.size());
            std::memcpy(buf.data()+off, p.username.data(), p.username.size());
        }
        
        buf.push_back(p.car_id);
        uint32_t pid_be = htonl(p.player_id);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &pid_be, 4);
        int32_t xbe = htonl(p.x), ybe = htonl(p.y);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &xbe, 4);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &ybe, 4);
        uint32_t ang_be = htonf32(p.angle);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &ang_be, 4);
        buf.push_back(p.health);
        uint32_t spd_be = htonf32(p.speed_mps);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &spd_be, 4);

        uint16_t xcp_be = htons(p.x_checkpoint);
        uint16_t ycp_be = htons(p.y_checkpoint);
        off = buf.size(); buf.resize(off + 2); std::memcpy(buf.data()+off, &xcp_be, 2);
        off = buf.size(); buf.resize(off + 2); std::memcpy(buf.data()+off, &ycp_be, 2);

        uint32_t hint_be = htonf32(p.hint_angle_deg);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &hint_be, 4);

        uint16_t pos_be = htons(p.position_in_race);
        off = buf.size(); buf.resize(off + 2); std::memcpy(buf.data()+off, &pos_be, 2);

        uint32_t dist_be = htonf32(p.distance_to_checkpoint);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &dist_be, 4);
    }
    
    uint32_t time_be = htonl(time_info.seconds);
    size_t off_time = buf.size(); buf.resize(off_time + 4); std::memcpy(buf.data()+off_time, &time_be, 4);
    // npcs
    buf.push_back((uint8_t)npcs.size());
    for (const auto& n : npcs) {
        buf.push_back(n.npc_id);
        int32_t xbe = htonl(n.x), ybe = htonl(n.y);
        size_t off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &xbe, 4);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &ybe, 4);
    }
    // events
    buf.push_back((uint8_t)events.size());
    for (const auto& e : events) {
        buf.push_back(e.event_type);
        uint16_t l = (uint16_t)e.username.size();
        uint16_t lbe = htons(l);
        size_t off = buf.size();
        buf.resize(off + 2); std::memcpy(buf.data()+off, &lbe, 2);
        if (l) {
            off = buf.size();
            buf.resize(off + e.username.size());
            std::memcpy(buf.data()+off, e.username.data(), e.username.size());
        }
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_result_race_current(const std::vector<PlayerResultCurrent>& current) {
    uint8_t code = CODE_S2C_RACE_RESULTS_CURRENT;
    uint8_t nplayers = (uint8_t)current.size();
    std::vector<uint8_t> buf;
    size_t usernames_total_len = 0;
    for (const auto& p : current) usernames_total_len += p.username.size();
    // Por jugador: player_id(4) + username_len(2) + race_time(4) + total_time(4) + position(1)
    // Más la suma real de usernames.
    buf.reserve(2 + nplayers * (4 + 2 + 4 + 4 + 1) + usernames_total_len);
    buf.push_back(code);
    buf.push_back(nplayers);
    // Formato: [player_id:4][username_len:2][username][race_time_seconds:4][total_time_seconds:4][position:1]
    for (const auto& p : current) {
        uint32_t pid_be = htonl(p.player_id);
        size_t off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &pid_be, 4);
        uint16_t l = (uint16_t)p.username.size();
        uint16_t lbe = htons(l);
        off = buf.size(); buf.resize(off + 2); std::memcpy(buf.data()+off, &lbe, 2);
        if (l) {
            off = buf.size(); buf.resize(off + p.username.size()); std::memcpy(buf.data()+off, p.username.data(), p.username.size());
        }
        uint32_t race_be = htonl(p.race_time_seconds);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &race_be, 4);
        uint32_t total_be = htonl(p.total_time_seconds);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &total_be, 4);
        buf.push_back(p.position);
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_improvement_ok(uint32_t player_id, uint8_t improvement_id, uint8_t success, uint32_t total_penalty_seconds) {
    uint8_t code = CODE_S2C_IMPROVEMENT;
    uint32_t pid_be = htonl(player_id);
    uint32_t penalty_be = htonl(total_penalty_seconds);
    std::vector<uint8_t> buf;
    // code(1)+player_id(4)+improvement_id(1)+success(1)+penalty(4)
    buf.reserve(1 + 4 + 1 + 1 + 4);
    buf.push_back(code);
    size_t off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &pid_be, 4);
    buf.push_back(improvement_id);
    buf.push_back(success);
    off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &penalty_be, 4);
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

ServerProtocol::ServerProtocol(Socket&& skt): skt(std::move(skt)) {
    init_recv_dispatch();
}

// ------------------ NUEVO: Dispatch init ------------------
void ServerProtocol::init_recv_dispatch() {
    recv_dispatch = {
        {CODE_C2S_NAME,        [this](){ return parse_name(); }},
        {CODE_C2S_MOVE,        [this](){ return parse_move(); }},
        {CODE_C2S_ROOM,        [this](){ return parse_room(); }},
        {CODE_C2S_START_GAME,  [this](){ return parse_start_game(); }},
        {CODE_C2S_CHOOSE_CAR,  [this](){ return parse_choose_car(); }},
        {CODE_C2S_IMPROVEMENT, [this](){ return parse_improvement(); }},
        {CODE_C2S_CHEAT,       [this](){ return parse_cheat(); }},
        {CODE_C2S_EXIT,        [this](){ return parse_exit(); }},
    };
}

//NUEVO: Handlers 
ClientMessage ServerProtocol::parse_name() {
    ClientMessage dto; dto.type = ClientMessage::Type::Name;
    uint16_t len_be=0; skt.recvall(&len_be,2);
    uint16_t len=ntohs(len_be);
    std::string username(len,'\0');
    if(len) skt.recvall(&username[0], len);
    dto.username = std::move(username);
    return dto;
}

ClientMessage ServerProtocol::parse_move() {
    ClientMessage dto; dto.type = ClientMessage::Type::Move;
    uint8_t mv=0; skt.recvall(&mv,1);
    dto.movement = (Movement)mv;
    return dto;
}

ClientMessage ServerProtocol::parse_room() {
    ClientMessage dto; dto.type = ClientMessage::Type::Room;
    uint8_t sub=0; skt.recvall(&sub,1);
    dto.room_cmd = sub;
    if(sub == ROOM_JOIN){
        uint8_t room=0; skt.recvall(&room,1);
        dto.room_id = room;
    }
    return dto;
}

ClientMessage ServerProtocol::parse_start_game() {
    ClientMessage dto; dto.type = ClientMessage::Type::StartGame;
    uint8_t qty=0; skt.recvall(&qty,1);
    std::vector<std::pair<std::string,uint8_t>> races; races.reserve(qty);
    for(uint8_t i=0;i<qty;++i){
        uint16_t lbe=0; skt.recvall(&lbe,2);
        uint16_t l=ntohs(lbe);
        std::string map(l,'\0');
        if(l) skt.recvall(&map[0], l);
        uint8_t route=0; skt.recvall(&route,1);
        races.emplace_back(std::move(map), route);
    }
    dto.races = std::move(races);
    return dto;
}

ClientMessage ServerProtocol::parse_choose_car() {
    ClientMessage dto; dto.type = ClientMessage::Type::ChooseCar;
    uint8_t car=0; skt.recvall(&car,1);
    dto.car_id = car;
    return dto;
}

ClientMessage ServerProtocol::parse_improvement() {
    ClientMessage dto; dto.type = ClientMessage::Type::Improvement;
    uint8_t imp=0; skt.recvall(&imp,1);
    dto.improvement = imp;
    return dto;
}

ClientMessage ServerProtocol::parse_cheat() {
    ClientMessage dto; dto.type = ClientMessage::Type::Cheat;
    uint8_t cheat=0; skt.recvall(&cheat,1);
    dto.cheat = cheat;
    return dto;
}

ClientMessage ServerProtocol::parse_exit() {
    ClientMessage dto; dto.type = ClientMessage::Type::Exit;
    return dto;
}

ClientMessage ServerProtocol::receive() {
    ClientMessage dto; dto.type = ClientMessage::Type::Unknown;
    uint8_t code=0;
    int r = skt.recvall(&code,1);
    if(r==0) return dto;

    auto it = recv_dispatch.find(code);
    if (it != recv_dispatch.end()) {
        return it->second();
    }
    return dto; // Unknown
}