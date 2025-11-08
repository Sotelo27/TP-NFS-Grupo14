#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <cstdint>
#include <string>

struct PlayerInfo {
    uint32_t player_id;
    std::string username;
    bool is_ready;
    uint8_t health;        // Vida del jugador (0-100)
    uint32_t race_time_ms; // Tiempo de carrera en milisegundos

    PlayerInfo() 
        : player_id(0), username(""), is_ready(false), health(100), race_time_ms(0) {}
    
    PlayerInfo(uint32_t id, const std::string& name, bool ready = false, 
               uint8_t hp = 100, uint32_t time_ms = 0)
        : player_id(id), username(name), is_ready(ready), health(hp), race_time_ms(time_ms) {}
};

#endif
