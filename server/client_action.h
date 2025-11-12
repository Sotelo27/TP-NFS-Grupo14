#ifndef CLIENT_ACTION_H
#define CLIENT_ACTION_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "../common/dto/movement.h"

struct ClientAction {
    enum class Type { Name, Move, Room, StartGame } type;
    size_t id;

    std::string username;
    Movement movement;

    // Acciones de sala
    uint8_t room_cmd{0};
    uint8_t room_id{0};
    
    // StartGame
    std::vector<std::pair<std::string, uint8_t>> races;

    ClientAction(size_t id, Movement mv): type(Type::Move), id(id), movement(mv) {}

    ClientAction(size_t id, std::string name):
            type(Type::Name), id(id), username(std::move(name)), movement() {}

    ClientAction(size_t id, uint8_t room_cmd, uint8_t room_id):
            type(Type::Room), id(id), movement(), room_cmd(room_cmd), room_id(room_id) {}
    
    //constructor para StartGame
    ClientAction(size_t id, std::vector<std::pair<std::string, uint8_t>> races):
            type(Type::StartGame), id(id), movement(), races(std::move(races)) {}

    ClientAction() = default;
};

#endif
