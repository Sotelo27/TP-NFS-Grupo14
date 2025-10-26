#include "server_protocol.h"

#include <arpa/inet.h>
#include <cstdint>
#include <string>
#include <cstring>
#include <vector>

void ServerProtocol::send_ok() {
    uint8_t code = CODE_S2C_OK;
    skt.sendall(&code, sizeof(code));
}

void ServerProtocol::send_pos(int16_t x, int16_t y) {
    uint8_t code = CODE_S2C_POS;
    uint16_t x_be = htons((uint16_t)x);
    uint16_t y_be = htons((uint16_t)y);

    std::vector<char> buf;
    buf.reserve(1 + 2 + 2);
    buf.push_back((char)code);
    buf.insert(buf.end(), (char*)&x_be, (char*)&x_be + 2);
    buf.insert(buf.end(), (char*)&y_be, (char*)&y_be + 2);

    skt.sendall(buf.data(), buf.size());
}

void ServerProtocol::enviar_mensaje(uint16_t cantidad_nitros_activos, uint8_t mensaje) {
    uint8_t codigo = CODE_SERVER_MSG;

    uint16_t cantidad_be = htons(cantidad_nitros_activos);

    std::vector<char> paquete(sizeof(codigo) + sizeof(cantidad_be) + sizeof(mensaje));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &codigo, sizeof(codigo));
    offset += sizeof(codigo);
    std::memcpy(paquete.data() + offset, &cantidad_be, sizeof(cantidad_be));
    offset += sizeof(cantidad_be);
    std::memcpy(paquete.data() + offset, &mensaje, sizeof(mensaje));

    skt.sendall(paquete.data(), paquete.size());
}

void ServerProtocol::enviar_rooms_default() {
    // Implementación por defecto: enviar un mensaje de tipo SERVER_MSG con 0 nitro y código 0
    uint8_t codigo = CODE_SERVER_MSG;
    uint16_t cantidad_be = htons((uint16_t)0);
    uint8_t mensaje = (uint8_t)ERROR_MESSAGE;

    std::vector<char> paquete(sizeof(codigo) + sizeof(cantidad_be) + sizeof(mensaje));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &codigo, sizeof(codigo));
    offset += sizeof(codigo);
    std::memcpy(paquete.data() + offset, &cantidad_be, sizeof(cantidad_be));
    offset += sizeof(cantidad_be);
    std::memcpy(paquete.data() + offset, &mensaje, sizeof(mensaje));

    skt.sendall(paquete.data(), paquete.size());
}

ClientMessage ServerProtocol::receive() {
    ClientMessage dto;

    uint8_t code = 0;
    int r = skt.recvall(&code, sizeof(code));
    if (r == ERROR) {
        dto.type = ClientMessage::Type::Unknown;
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
        return dto;
    }

    if (code == CODE_C2S_MOVE) {
        uint8_t mv = 0;
        skt.recvall(&mv, sizeof(mv));

        dto.type = ClientMessage::Type::Move;
        dto.movement = (Movement)mv;  
        return dto;
    }

    dto.type = ClientMessage::Type::Unknown;
    return dto;
}
