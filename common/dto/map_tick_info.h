#ifndef DTO_MAP_TICK_INFO_H
#define DTO_MAP_TICK_INFO_H

#include <cstdint>
#include <string>
#include <vector>

struct PlayerTickInfo {
    std::string username;
    uint8_t car_id;
    int32_t x;
    int32_t y;
    float angle;        // NUEVO: ángulo del auto
    uint8_t health;     // NUEVO: vida del jugador
    uint16_t speed;     // NUEVO: velocidad actual
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

// NUEVO: Estructura completa del tick con timestamp
struct GameTickInfo {
    uint32_t timestamp_ms;  // Tiempo en milisegundos desde inicio de carrera
    std::vector<PlayerTickInfo> players;
    std::vector<NpcTickInfo> npcs;
    std::vector<EventInfo> events;
};

#endif
