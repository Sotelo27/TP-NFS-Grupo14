#ifndef DTO_RESULTS_INFO_H
#define DTO_RESULTS_INFO_H

#include <cstdint>
#include <string>

struct PlayerResultCurrent {
    std::string username;
    uint16_t time_seconds;
};

struct PlayerResultTotal {
    std::string username;
    uint32_t total_time_seconds;
};

#endif
