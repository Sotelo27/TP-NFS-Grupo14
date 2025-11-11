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

void ServerProtocol::send_pos(uint32_t id, int16_t x, int16_t y, float angle) {
    uint8_t code = CODE_S2C_POS;

    uint32_t id_be = htonl(id);
    uint16_t x_be = htons((uint16_t)x);
    uint16_t y_be = htons((uint16_t)y);
    uint32_t ang_be = htonf32(angle);

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

void ServerProtocol::send_your_id(uint32_t id) {
    uint8_t code = CODE_S2C_YOUR_ID;
    uint32_t id_be = htonl(id);

    uint8_t buf[1 + 4];
    buf[0] = code;
    std::memcpy(buf + 1, &id_be, 4);
    skt.sendall(buf, sizeof(buf));
}

void ServerProtocol::send_player_name(uint32_t id, const std::string& username) {
    uint8_t code = CODE_S2C_PLAYER_NAME;
    uint32_t id_be = htonl(id);
    uint16_t len = (uint16_t)username.size();
    uint16_t len_be = htons(len);

    std::vector<uint8_t> buf;
    buf.reserve(1 + 4 + 2 + username.size());
    buf.push_back(code);

    size_t off = buf.size();
    buf.resize(off + 4); std::memcpy(buf.data() + off, &id_be, 4);
    off = buf.size();
    buf.resize(off + 2); std::memcpy(buf.data() + off, &len_be, 2);
    if (len > 0) {
        off = buf.size();
        buf.resize(off + username.size());
        std::memcpy(buf.data() + off, username.data(), username.size());
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

void ServerProtocol::send_room_created(uint8_t room_id) {
    uint8_t code = CODE_S2C_ROOM_CREATED;
    uint8_t buf[2] = {code, room_id};
    skt.sendall(buf, sizeof(buf));
}

void ServerProtocol::send_players_list(const std::vector<PlayerInfo>& players) {
    uint8_t code = CODE_S2C_PLAYERS_LIST;
    uint8_t count = (uint8_t)players.size();
    
    std::vector<uint8_t> buf;
    buf.reserve(2 + players.size() * 30); // Estimación aumentada por campos extra
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

void ServerProtocol::enviar_mensaje(uint16_t cantidad_nitros_activos, uint8_t mensaje) {
    uint8_t codigo = CODE_SERVER_MSG;

    uint16_t cantidad_be = htons(cantidad_nitros_activos);

    std::vector<uint8_t> paquete(sizeof(codigo) + sizeof(cantidad_be) + sizeof(mensaje));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &codigo, sizeof(codigo));
    offset += sizeof(codigo);
    std::memcpy(paquete.data() + offset, &cantidad_be, sizeof(cantidad_be));
    offset += sizeof(cantidad_be);
    std::memcpy(paquete.data() + offset, &mensaje, sizeof(mensaje));

    skt.sendall(paquete.data(), (unsigned int)paquete.size());
}

void ServerProtocol::enviar_rooms_default() {
    uint8_t codigo = CODE_SERVER_MSG;
    uint16_t cantidad_be = htons((uint16_t)0);
    uint8_t mensaje = (uint8_t)ERROR_MESSAGE;

    std::vector<uint8_t> paquete(sizeof(codigo) + sizeof(cantidad_be) + sizeof(mensaje));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &codigo, sizeof(codigo));
    offset += sizeof(codigo);
    std::memcpy(paquete.data() + offset, &cantidad_be, sizeof(cantidad_be));
    offset += sizeof(cantidad_be);
    std::memcpy(paquete.data() + offset, &mensaje, sizeof(mensaje));

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

void ServerProtocol::send_race_start(const std::string& map, uint8_t amount_checkpoints,
                                     const std::vector<std::pair<int32_t,int32_t>>& checkpoints) {
    uint8_t code = CODE_S2C_RACE_START;
    uint16_t len = (uint16_t)map.size();
    uint16_t len_be = htons(len);

    std::vector<uint8_t> buf;
    buf.reserve(1 + 2 + map.size() + 1 + checkpoints.size()*8);
    buf.push_back(code);

    size_t off = buf.size();
    buf.resize(off + 2); std::memcpy(buf.data()+off, &len_be, 2);
    if (len) {
        off = buf.size();
        buf.resize(off + map.size()); std::memcpy(buf.data()+off, map.data(), map.size());
    }
    buf.push_back(amount_checkpoints);
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
        uint16_t l = (uint16_t)p.username.size();
        uint16_t lbe = htons(l);
        size_t off = buf.size();
        buf.resize(off + 2); std::memcpy(buf.data()+off, &lbe, 2);
        if (l) {
            off = buf.size();
            buf.resize(off + p.username.size());
            std::memcpy(buf.data()+off, p.username.data(), p.username.size());
        }
        uint16_t time_be = htons(p.time_seconds);
        off = buf.size();
        buf.resize(off + 2); std::memcpy(buf.data()+off, &time_be, 2);
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
    }
    skt.sendall(buf.data(), (unsigned int)buf.size());
}

void ServerProtocol::send_map_info(const std::vector<PlayerTickInfo>& players,
                                   const std::vector<NpcTickInfo>& npcs,
                                   const std::vector<EventInfo>& events) {
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
        int32_t xbe = htonl(p.x), ybe = htonl(p.y);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &xbe, 4);
        off = buf.size(); buf.resize(off + 4); std::memcpy(buf.data()+off, &ybe, 4);
    }
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

ClientMessage ServerProtocol::receive() {
    ClientMessage dto;
    dto.type = ClientMessage::Type::Unknown;

    uint8_t code = 0;
    int r = skt.recvall(&code, sizeof(code));
    if (r == 0) {
        return dto;
    }

    if (code == CODE_C2S_NAME) {
        uint16_t len_be = 0;
        skt.recvall(&len_be, sizeof(len_be));
        uint16_t len = ntohs(len_be);

        std::string username;
        username.resize(len);
        if (len > 0) {
            skt.recvall(username.data(), len);
        }

        dto.type = ClientMessage::Type::Name;
        dto.username = std::move(username);
    } else if (code == CODE_C2S_MOVE) {
        uint8_t mv = 0;
        skt.recvall(&mv, sizeof(mv));

        dto.type = ClientMessage::Type::Move;
        dto.movement = (Movement)mv;
        
    } else if (code == CODE_C2S_ROOM) {
        uint8_t sub = 0;
        skt.recvall(&sub, sizeof(sub));
        dto.type = ClientMessage::Type::Room;
        dto.room_cmd = sub;
        if (sub == ROOM_JOIN) {
            uint8_t room = 0;
            skt.recvall(&room, sizeof(room));
            dto.room_id = room;
        }
    } else if (code == CODE_C2S_START_GAME) {
        // 0x0A <QUANTITY-RACES u8> [ <LENGTH u16> <MAP bytes> <ROUTE u8> ]...
        uint8_t qty = 0;
        skt.recvall(&qty, sizeof(qty));
        std::vector<std::pair<std::string, uint8_t>> races;
        races.reserve(qty);
        for (uint8_t i = 0; i < qty; ++i) {
            uint16_t lbe = 0;
            skt.recvall(&lbe, sizeof(lbe));
            uint16_t l = ntohs(lbe);
            std::string map(l, '\0');
            if (l) {
                skt.recvall(&map[0], l);
            }
            uint8_t route = 0;
            skt.recvall(&route, sizeof(route));
            races.emplace_back(std::move(map), route);
        }
        dto.type = ClientMessage::Type::StartGame;
        dto.races = std::move(races);
    } else if (code == CODE_C2S_CHOOSE_CAR) {
        // 0x0B <CAR-ID u8>
        uint8_t car = 0;
        skt.recvall(&car, 1);
        dto.type = ClientMessage::Type::ChooseCar;
        dto.car_id = car;
    } else if (code == CODE_C2S_IMPROVEMENT) {
        // 0x0C <IMPROVEMENT u8>
        uint8_t imp = 0;
        skt.recvall(&imp, 1);
        dto.type = ClientMessage::Type::Improvement;
        dto.improvement = imp;
    } else if (code == CODE_C2S_CHEAT) {
        // 0x0D <CHEAT u8>
        uint8_t cheat = 0;
        skt.recvall(&cheat, 1);
        dto.type = ClientMessage::Type::Cheat;
        dto.cheat = cheat;
    } else if (code == CODE_C2S_EXIT) {
        dto.type = ClientMessage::Type::Exit;
    }

    return dto;
}
