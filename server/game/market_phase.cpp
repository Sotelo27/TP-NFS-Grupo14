#include "market_phase.h"
#include <cmath>

void MarketPhase::begin(std::map<size_t, Player>& players, float duration_seconds) {
    time_remaining = duration_seconds;
    pending_init = true;
    for (auto& [pid, player] : players) {
        (void)pid;
        player.reset_current_to_base();
    }
}

void MarketPhase::update(float dt) {
    time_remaining -= dt;
}

float MarketPhase::get_time_remaining() const {
    return time_remaining;
}

bool MarketPhase::consume_pending_market_init(const std::map<size_t, Player>& players, std::vector<ImprovementResult>& out) {
    if (!pending_init) return false;
    pending_init = false;

    out.clear();
    out.reserve(players.size());
    for (const auto& kv : players) {
        size_t pid = kv.first;
        PlayerMarketInfo m_info = market.get_total_player_info(pid);
        ImprovementResult msg;
        msg.player_id             = (uint32_t)(pid);
        msg.improvement_id        = (uint8_t)(CarImprovement::Init);
        msg.ok                    = true;
        msg.total_penalty_seconds = 0;
        msg.current_balance       = (uint32_t)(std::round(m_info.balance));
        out.push_back(msg);
    }
    return true;
}

bool MarketPhase::buy_upgrade(size_t player_id, CarImprovement improvement) {
    return market.buy_upgrade(player_id, improvement);
}

float MarketPhase::get_improvement_penalty(CarImprovement imp) const {
    return market.get_improvement_time_penalty(imp);
}

PlayerMarketInfo MarketPhase::get_player_market_info(size_t player_id) const {
    return market.get_total_player_info(player_id);
}

void MarketPhase::apply_upgrades_to_all(std::map<size_t, Player>& players) const {
    for (auto& kv : players) {
        size_t pid = kv.first;
        Player& player = kv.second;
        CarModel updated = market.apply_upgrades_to_model(pid, player.get_car_model());
        player.set_car_model(updated);
    }
}

std::unordered_map<size_t, float> MarketPhase::consume_penalties_for_race() {
    return market.consume_penalties_for_race();
}
