#ifndef CLIENT_ACTION_H
#define CLIENT_ACTION_H

#include <cstddef>
#include <cstdint>
#include "../common/base_protocol.h"

struct ClientAction {
    enum class Type { Name, Move, Room } type;
    size_t id;

    std::string username;
    Movement movement;

    // Acciones de sala
    uint8_t room_cmd{0};
    uint8_t room_id{0};

    ClientAction(size_t id, Movement mv)
        : type(Type::Move), id(id), movement(mv) {}

    ClientAction(size_t id, std::string name)
        : type(Type::Name), id(id), username(std::move(name)) {}

    ClientAction(size_t id, uint8_t room_cmd, uint8_t room_id)
        : type(Type::Room), id(id), room_cmd(room_cmd), room_id(room_id) {}

    ClientAction() = default;
};

#endif
