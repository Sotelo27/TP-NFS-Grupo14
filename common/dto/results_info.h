#ifndef DTO_RESULTS_INFO_H
#define DTO_RESULTS_INFO_H

#include <cstdint>
#include <string>

struct PlayerResultCurrent {
    uint32_t player_id;
    std::string username;
    uint32_t race_time_seconds;
    uint32_t total_time_seconds;
    uint8_t position; //posición en la carrera
};

struct PlayerResultTotal {
    std::string username;
    uint32_t total_time_seconds;
    uint8_t position; //posición total
};

#endif
