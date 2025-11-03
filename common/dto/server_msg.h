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
        YourId,
        Unknown,
        PlayerName,
        GameOver
    } type{Type::Unknown};

    uint32_t id{0};
    int16_t x{0}, y{0};
    float angle{0.0f};

    // Listado de salas
    std::vector<RoomInfo> rooms;

    // Nuevo: para CODE_S2C_PLAYER_NAME
    std::string username;
};

#endif
