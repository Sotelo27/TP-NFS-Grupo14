#ifndef DTO_SERVER_MSG_H
#define DTO_SERVER_MSG_H

#include <cstdint>
#include <vector>

#include "room_info.h"

struct ServerMessage {
    enum class Type { Ok, Pos, Rooms, YourId, Unknown } type = Type::Unknown;
    uint32_t id = 0;
    int16_t x = 0;
    int16_t y = 0;
    float angle = 0.f;

    // Listado de salas
    std::vector<RoomInfo> rooms;
};

#endif
