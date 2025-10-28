#include "server_protocol.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <arpa/inet.h>

void ServerProtocol::send_ok() {
    uint8_t code = CODE_S2C_OK;
    skt.sendall(&code, sizeof(code));
}

void ServerProtocol::send_pos(uint32_t id, int16_t x, int16_t y) {
    uint8_t code = CODE_S2C_POS;

    uint32_t id_be = htonl(id);
    uint16_t x_be = htons((uint16_t)x);
    uint16_t y_be = htons((uint16_t)y);

    std::vector<char> buf;
    buf.reserve(1 + 4 + 2 + 2);
    buf.push_back((char)(code));

    size_t offset = buf.size();
    buf.resize(offset + 4);
    std::memcpy(buf.data() + offset, &id_be, 4);

    offset = buf.size();
    buf.resize(offset + 2);
    std::memcpy(buf.data() + offset, &x_be, 2);

    offset = buf.size();
    buf.resize(offset + 2);
    std::memcpy(buf.data() + offset, &y_be, 2);

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
    }

    return dto;
}
