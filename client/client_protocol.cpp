#include "client_protocol.h"

#include <cstdint>
#include <vector>

#include <arpa/inet.h>

#define MOVE_BUF 2

void ClientProtocol::send_name(const std::string& username) {
    uint8_t code = CODE_C2S_NAME;
    uint16_t len = username.size();
    uint16_t len_be = htons(len);

    std::vector<char> buf;
    buf.reserve(1 + 2 + username.size());

    buf.push_back((char)code);
    buf.insert(buf.end(), (char*)&len_be, (char*)&len_be + 2);
    if (len > 0) {
        buf.insert(buf.end(), username.data(), username.data() + username.size());
    }

    skt.sendall(buf.data(), buf.size());
}

void ClientProtocol::send_move(Movement mov) {
    uint8_t code = CODE_C2S_MOVE;
    uint8_t mv = mov;
    char buf[MOVE_BUF] = {(char)code, (char)mv};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_create_room() {
    uint8_t code = CODE_C2S_ROOM;
    uint8_t sub = ROOM_CREATE;
    char buf[2] = {static_cast<char>(code), static_cast<char>(sub)};
    skt.sendall(buf, sizeof(buf));
}

void ClientProtocol::send_join_room(uint8_t room_id) {
    uint8_t code = CODE_C2S_ROOM;
    uint8_t sub = ROOM_JOIN;
    char buf[3] = {static_cast<char>(code), static_cast<char>(sub), static_cast<char>(room_id)};
    skt.sendall(buf, sizeof(buf));
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

        skt.recvall(&id_be, sizeof(id_be));
        skt.recvall(&x_be, sizeof(x_be));
        skt.recvall(&y_be, sizeof(y_be));

        dto.id = ntohl(id_be);
        dto.x = ntohs(x_be);
        dto.y = ntohs(y_be);
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
    }

    return dto;
}
