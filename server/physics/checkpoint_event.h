#ifndef PHYSICS_CHECKPOINT_EVENT_H
#define PHYSICS_CHECKPOINT_EVENT_H

#include <cstddef>
#include <string>

struct CheckpointEvent {
    size_t car_id;
    std::string race_id;
    size_t checkpoint_index;

    CheckpointEvent() = default;
    CheckpointEvent(size_t car, std::string race, size_t idx)
        : car_id(car), race_id(std::move(race)), checkpoint_index(idx) {}
};

#endif
