#ifndef CAR_INFO_GAME_H
#define CAR_INFO_GAME_H

#include "utils/Area.h"
#include "../../common/dto/map_tick_info.h"

struct CarInfoGame {
    PlayerTickInfo info_car;
    Area dest_area;
};

#endif
