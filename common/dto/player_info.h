#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <cstdint>
#include <string>

struct PlayerInfo {
    uint32_t player_id;
    std::string username;
    bool is_ready;
    bool is_admin;         // Indica si es el creador/admin de la sala
    uint8_t health;        // Vida del jugador (0-100)
    uint32_t race_time_ms; // Tiempo de carrera en milisegundos

    PlayerInfo() 
        : player_id(0), username(""), is_ready(false), is_admin(false), health(100), race_time_ms(0) {}
    
    PlayerInfo(uint32_t id, const std::string& name, bool ready = false, 
               bool admin = false, uint8_t hp = 100, uint32_t time_ms = 0)
        : player_id(id), username(name), is_ready(ready), is_admin(admin), health(hp), race_time_ms(time_ms) {}
};

#endif
