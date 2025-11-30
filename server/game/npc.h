#ifndef GAME_NPC_H
#define GAME_NPC_H

#include <cstdint>

struct Npc {
    uint8_t npc_id;
    float x_m;
    float y_m;
    float vx{0.f}; // velocidad en x (m/s)
    float vy{0.f}; // velocidad en y (m/s)
};

#endif // GAME_NPC_H
