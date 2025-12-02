#ifndef PHYSICS_DAMAGE_EVENT_H
#define PHYSICS_DAMAGE_EVENT_H

#include <cstddef>

struct DamageEvent {
    size_t player_id;
    float impulse;

    DamageEvent() = default;
    DamageEvent(size_t player, float imp)
        : player_id(player), impulse(imp) {}
};

#endif
