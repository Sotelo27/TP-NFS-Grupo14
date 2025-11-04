#ifndef DTO_SERVER_MSG_H
#define DTO_SERVER_MSG_H

#include <cstdint>
#include <string>
#include <vector>

#include "room_info.h"

struct ServerMessage {
    enum class Type {
        Ok,
        Pos,
        Rooms,
        PlayerName,
        YourId,
        GameOver,
        Unknown
    } type{Type::Unknown};

    // Campos comunes / reutilizados por varios tipos
    uint32_t id{0};

    // Para Pos
    uint16_t x{0};
    uint16_t y{0};
    float angle{0.0f}; // agregado: ángulo en radianes (S2C_POS)

    // Para Rooms
    std::vector<RoomInfo> rooms;

    // Para PlayerName
    std::string username;
};

#endif
