#ifndef DTO_ROOM_INFO_H
#define DTO_ROOM_INFO_H

#include <cstdint>

struct RoomInfo {
    uint8_t id = 0;
    uint8_t current_players = 0;
    uint8_t max_players = 0;
};

#endif
