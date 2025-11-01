#ifndef DTO_CLIENT_MSG_H
#define DTO_CLIENT_MSG_H

#include <cstdint>
#include <string>

#include "movement.h"

struct ClientMessage {
    enum class Type { Name, Move, Room, Unknown } type = Type::Unknown;

    std::string username;
    Movement movement = Movement::Up;

    // Sala: subcódigo y room id (si join)
    uint8_t room_cmd = 0;  // ROOM_CREATE | ROOM_JOIN
    uint8_t room_id = 0;   // válido con ROOM_JOIN
};

#endif
