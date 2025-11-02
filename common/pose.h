#ifndef POSE_H
#define POSE_H

#include <cstdint>

struct Pose {
    int16_t x{0};
    int16_t y{0};
    float angle{0.f};
};

#endif
