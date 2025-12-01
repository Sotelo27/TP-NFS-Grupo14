#include "result.h"
#include <algorithm>

std::vector<PlayerResultCurrent> Result::build_player_result_current(
    const std::map<size_t, Player>& players,
    const RaceResult& race_result,
    const std::unordered_map<size_t, float>& penalties_seconds) const {
    std::vector<PlayerResultCurrent> packed;
    packed.reserve(race_result.result.size());

    for (const auto& entry : race_result.result) {
        auto player_it = players.find(entry.player_id);
        if (player_it == players.end()) {
            continue;
        }

        const Player& player = player_it->second;

        float base_seconds = entry.finish_time_seconds;

        float penalty_seconds = 0.f;
        auto penalty_it = penalties_seconds.find(entry.player_id);
        if (penalty_it != penalties_seconds.end()) {
            penalty_seconds = (float)(penalty_it->second);
        }

        float race_time = base_seconds + penalty_seconds;
        float total_time = player.get_total_time_seconds();

        PlayerResultCurrent result{};
        result.player_id          = entry.player_id;
        result.username           = player.get_name();
        result.race_time_seconds  = (uint32_t)(race_time);
        result.total_time_seconds = (uint32_t)(total_time);
        result.position           = (uint8_t)(entry.position);

        packed.push_back(result);
    }

    return packed;
}

std::vector<PlayerResultTotal> Result::build_total_results(
    const std::map<size_t, Player>& players) const {
    std::vector<std::pair<std::string, float>> totals;
    totals.reserve(players.size());
    for (const auto& kv : players) {
        const Player& p = kv.second;
        totals.emplace_back(p.get_name(), p.get_total_time_seconds());
    }
    std::sort(totals.begin(), totals.end(), [](const auto& a, const auto& b){ return a.second < b.second; });

    std::vector<PlayerResultTotal> out;
    out.reserve(totals.size());
    uint8_t pos = 1;
    for (const auto& t : totals) {
        PlayerResultTotal r{};
        r.username = t.first;
        r.total_time_seconds = static_cast<uint32_t>(t.second);
        r.position = pos++;
        out.push_back(r);
    }
    return out;
}

void Result::set_pending_results(std::vector<PlayerResultCurrent>&& current) {
    last_results_current = std::move(current);
    pending_results = true;
}

bool Result::has_pending_results() const {
    return pending_results;
}

bool Result::consume_pending_results(std::vector<PlayerResultCurrent>& out) {
    if (!pending_results) return false;
    out = std::move(last_results_current);
    pending_results = false;
    return true;
}

void Result::set_pending_total_results(std::vector<PlayerResultTotal>&& total) {
    last_results_total = std::move(total);
    pending_total_results = true;
}

bool Result::has_pending_total_results() const {
    return pending_total_results;
}

bool Result::consume_pending_total_results(std::vector<PlayerResultTotal>& out) {
    if (!pending_total_results) return false;
    out = std::move(last_results_total);
    pending_total_results = false;
    return true;
}
