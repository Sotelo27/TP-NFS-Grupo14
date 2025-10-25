#include "client_protocol.h"
#include <arpa/inet.h>
#include <cstdint>
#include <vector>

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

ServerMessage ClientProtocol::receive() {
    ServerMessage dto;

    uint8_t code = 0;
    int r = skt.recvall(&code, sizeof(code));
    if (r == ERROR) {
        dto.type = ServerMessage::Type::Unknown;
        return dto;
    }

    if (code == CODE_S2C_OK) {
        dto.type = ServerMessage::Type::Ok;
        return dto;
    }

    if (code == CODE_S2C_POS) {
        dto.type = ServerMessage::Type::Pos;

        uint16_t x_be = 0, y_be = 0;
        skt.recvall(&x_be, sizeof(x_be));
        skt.recvall(&y_be, sizeof(y_be));

        dto.x = ntohs(x_be); 
        dto.y = ntohs(y_be);
        return dto;
    }

    dto.type = ServerMessage::Type::Unknown;
    return dto;
}

