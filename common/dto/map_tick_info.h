#ifndef DTO_MAP_TICK_INFO_H
#define DTO_MAP_TICK_INFO_H

#include <cstdint>
#include <string>
#include <vector>
#include "../enum/car_improvement.h"

struct PlayerTickInfo {
    std::string username;
    uint8_t car_id;
    uint32_t player_id;
    int32_t x;
    int32_t y;
    float angle;
    uint8_t health;
    float speed_mps;
    
    uint16_t x_checkpoint;
    uint16_t y_checkpoint;
    float hint_angle_deg;

    uint16_t position_in_race;
    float distance_to_checkpoint;

    uint8_t max_health;
    uint16_t checkpoints_remaining;

    std::vector<CarImprovement> improvements;
};

struct NpcTickInfo {
    uint8_t npc_id;
    int32_t x;
    int32_t y;
    float angle{0.f};
};

struct EventInfo {
    uint8_t event_type;
    std::string username;
};

// Información de tiempo de carrera para enviar en ticks
struct TimeTickInfo {
    uint32_t seconds; // segundos transcurridos de la carrera
};

#endif
