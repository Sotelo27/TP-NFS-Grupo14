#ifndef DTO_MAP_TICK_INFO_H
#define DTO_MAP_TICK_INFO_H

#include <cstdint>
#include <string>

struct PlayerTickInfo {
    std::string username;
    uint8_t car_id;
    uint32_t player_id;
    int32_t x;
    int32_t y;
    float angle;
    uint8_t health;
};

struct NpcTickInfo {
    uint8_t npc_id;
    int32_t x;
    int32_t y;
};

struct EventInfo {
    uint8_t event_type;
    std::string username;
};

#endif
