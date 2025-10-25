#ifndef BASE_PROTOCOL_H
#define BASE_PROTOCOL_H

#include <cstdint>
#include <string>

enum class Movement : uint8_t {
    Up = 0x01,
    Down = 0x02,
    Left = 0x03,
    Right = 0x04
};

struct ClientMessage {
    enum class Type { Name, Move, Unknown } type = Type::Unknown;

    std::string username;
    Movement movement = Movement::Up;
};

struct ServerMessage {
    enum class Type { Ok, Pos, Unknown } type = Type::Unknown;

    int16_t x = 0;
    int16_t y = 0;
};

#endif
