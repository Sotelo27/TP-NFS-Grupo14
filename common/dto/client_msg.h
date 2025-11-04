#ifndef DTO_CLIENT_MSG_H
#define DTO_CLIENT_MSG_H

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "movement.h"

struct ClientMessage {
    enum class Type {
        Name,
        Pos,
        Move,
        Room,
        Exit,
        StartGame,
        ChooseCar,
        Improvement,
        Cheat,
        Unknown
    } type = Type::Unknown;

    // Identidad / datos básicos
    std::string username{};
    Movement movement{};

    uint16_t x{0}; // pixeles
    uint16_t y{0}; // pixeles
    float angle{0.0f}; //angulo ya en grados

    // Acciones de sala
    uint8_t room_cmd{0};
    uint8_t room_id{0};

    // StartGame: vector de carreras (map, route)
    std::vector<std::pair<std::string, uint8_t>> races;

    // ChooseCar
    uint8_t car_id{0};

    // Improvement
    uint8_t improvement{0};

    // Cheat
    uint8_t cheat{0};
};

#endif
