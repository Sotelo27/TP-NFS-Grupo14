#ifndef DTO_CAR_INFO_H
#define DTO_CAR_INFO_H

#include <cstdint>

struct CarInfo {
    uint8_t id;
    uint8_t speed;
    uint8_t acceleration;
    uint8_t health;
    uint8_t mass;
    uint8_t controllability;
};

#endif
