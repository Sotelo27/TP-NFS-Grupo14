#include "market.h"
#include <algorithm>
#include <cmath>

Market::Market(): catalog_upgrades(), player_market_info() {
    catalog_upgrades.emplace(CarImprovement::Health, UpgradeInfo{60.0f, 50.0f});
    catalog_upgrades.emplace(CarImprovement::Acceleration, UpgradeInfo{180.0f, 4.0f});
    catalog_upgrades.emplace(CarImprovement::Speed, UpgradeInfo{120.0f, 0.5f});
    catalog_upgrades.emplace(CarImprovement::Controllability, UpgradeInfo{90.0f, 2.0f});
    catalog_upgrades.emplace(CarImprovement::Mass, UpgradeInfo{30.0f, 50.0f});
}

void Market::reset_upgrades() {
    player_market_info.clear();
}

const UpgradeInfo& Market::find_info_upgrade(CarImprovement id) const {
    return catalog_upgrades.at(id);
}

bool Market::buy_upgrade(std::size_t player_id, CarImprovement id) {
    const UpgradeInfo& upgrade = find_info_upgrade(id);

    auto& info = player_market_info[player_id];

    // si ya la tiene, no la vuelve a comprar
    auto it = std::find(info.upgrades.begin(), info.upgrades.end(), id);
    if (it != info.upgrades.end()) {
        return false;
    }

    info.upgrades.push_back(id);
    info.total_time_penalty += upgrade.time_penalty;
    return true;
}

CarModel Market::apply_upgrades_to_model(std::size_t player_id, const CarModel& base) const {
    CarModel result = base;

    auto it = player_market_info.find(player_id);
    if (it == player_market_info.end()) {
        return result; // no encontro es porque no compro nada
    }

    const PlayerMarketInfo& p_info_market = it->second;

    for (CarImprovement id : p_info_market.upgrades) {
        const UpgradeInfo& upgrade = find_info_upgrade(id);

        switch (id) {
        case CarImprovement::Health:
            result.life += upgrade.value;
            break;
        case CarImprovement::Acceleration:
            result.fuerzaAceleracionN += upgrade.value;
            break;
        case CarImprovement::Speed:
            result.velocidadMaxMps += upgrade.value;
            break;
        case CarImprovement::Controllability:
            result.torqueGiro += upgrade.value;
            break;
        case CarImprovement::Mass:
            result.masaKg = std::max(0.0f, result.masaKg - upgrade.value);
            break;
        default:
            break;
        }
    }

    return result;
}

std::unordered_map<std::size_t, float> Market::consume_penalties_for_race(){
    std::unordered_map<std::size_t, float> penalties_seconds; // player_id -> seconds de penelizacion

    for (const auto& kv : player_market_info) {
        std::size_t player_id          = kv.first;
        const PlayerMarketInfo& pinfo  = kv.second;

        if (pinfo.total_time_penalty <= 0.0f) continue;

        penalties_seconds.emplace(player_id, pinfo.total_time_penalty);

    }

    // una vez consumido lo reseteo
    // player_market_info.clear();
    reset_upgrades();

    return penalties_seconds;
}

PlayerMarketInfo Market::get_total_player_info(size_t player_id) const{
    auto it = player_market_info.find(player_id);
    if (it == player_market_info.end()) {
        return PlayerMarketInfo{};
    }
    return it->second;
}
