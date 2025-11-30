#ifndef INFO_GAME_H
#define INFO_GAME_H

#include "utils/Area.h"
#include "../../common/dto/map_tick_info.h"

struct CarInfoGame {
    PlayerTickInfo info_car;
    Area dest_area;
};

struct NpcInfoGame {
    NpcTickInfo info_npc;
    Area dest_area;
};

#endif
