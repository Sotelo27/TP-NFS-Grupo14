#ifndef MARKET_INFO_H
#define MARKET_INFO_H

#include <cstdint>
#include <vector>
#include "../../common/enum/car_improvement.h"


// info de cada mejora, cuanto penaliza y cuanto vale
struct UpgradeInfo {
    float time_penalty;
    float value;
};

// lo que compra cada jugador
struct PlayerMarketInfo {
    float total_time_penalty = 0.f;
    std::vector<CarImprovement> upgrades;
};


#endif