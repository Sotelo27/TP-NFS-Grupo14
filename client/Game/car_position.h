#ifndef CAR_POSITION_H
#define CAR_POSITION_H

#include "utils/Area.h"

struct Position {
    int x_car_map;
    int y_car_map;
    float angle;
};

struct CarPosition {
    Position position;
    Area dest_area;
};

#endif
