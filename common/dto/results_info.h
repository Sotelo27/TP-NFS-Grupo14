#ifndef DTO_RESULTS_INFO_H
#define DTO_RESULTS_INFO_H

#include <cstdint>
#include <string>

struct PlayerResultCurrent {
    uint32_t player_id;
    std::string username;
    uint32_t race_time_seconds;
    uint32_t total_time_seconds;
    uint8_t position;
};

struct PlayerResultTotal {
    std::string username;
    uint32_t total_time_seconds;
    uint8_t position;
};

struct ImprovementResult {
    uint32_t player_id;
    uint8_t  improvement_id;
    bool     ok;
    uint32_t total_penalty_seconds;
    uint32_t current_balance; 
};

#endif
