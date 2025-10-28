#ifndef CLIENT_ACTION_H
#define CLIENT_ACTION_H

#include <cstddef>
#include <cstdint>
#include "../common/base_protocol.h"

struct ClientAction {
    enum class Type { Name, Move } type;
    size_t id;

    std::string username;
    Movement movement;

    ClientAction(size_t id, Movement mv)
        : type(Type::Move), id(id), movement(mv) {}

    ClientAction(size_t id, std::string name)
        : type(Type::Name), id(id), username(std::move(name)) {}

    ClientAction() = default;
};

#endif
