#ifndef DTO_RESULTS_INFO_H
#define DTO_RESULTS_INFO_H

#include <cstdint>
#include <string>

struct PlayerResultCurrent {
    std::string username;
    uint16_t time_seconds;
    uint8_t position; //posición en la carrera
};

struct PlayerResultTotal {
    std::string username;
    uint32_t total_time_seconds;
    uint8_t position; //posición total
};

#endif
