#ifndef CAR_IMPROVEMENT_H
#define CAR_IMPROVEMENT_H

#include <cstdint>

enum class CarImprovement : uint8_t {
    Health = 0,
    Speed = 1,
    Acceleration = 2,
    Mass = 3,
    Controllability = 4,
    Nitro = 5,
    Init = 6
    
};

#endif
