#ifndef DTO_MOVEMENT_H
#define DTO_MOVEMENT_H

#include <cstdint>

enum Movement : uint8_t {
    Up = 0x01,
    Down = 0x02,
    Left = 0x03,
    Right = 0x04,
};

#endif
