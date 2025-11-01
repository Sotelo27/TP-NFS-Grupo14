#ifndef BASE_PROTOCOL_H
#define BASE_PROTOCOL_H

#include <cstdint>
#include <string>
#include <vector>

enum Movement : uint8_t {
    Up = 0x01,
    Down = 0x02,
    Left = 0x03,
    Right = 0x04
};

// Información de una sala (para listado del server)
struct RoomInfo {
    uint8_t id = 0;
    uint8_t current_players = 0;
    uint8_t max_players = 0;
};

struct ClientMessage {
    enum class Type { Name, Move, Room, Unknown } type = Type::Unknown;

    std::string username;
    Movement movement = Movement::Up;

    // Sala: subcódigo y room id (si join)
    uint8_t room_cmd = 0;  // ROOM_CREATE | ROOM_JOIN
    uint8_t room_id = 0;   // válido con ROOM_JOIN
};

struct ServerMessage {
    enum class Type { Ok, Pos, Rooms, Unknown } type = Type::Unknown;
    uint32_t id = 0;
    int16_t x = 0;
    int16_t y = 0;

    // Listado de salas
    std::vector<RoomInfo> rooms;
};

#endif
