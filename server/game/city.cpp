#include "city.h"
#include <algorithm>
#include <iostream>

City::City() : physics_world() {}

PhysicsWorld& City::get_world() {
    return physics_world;
}

void City::step(float dt) {
    physics_world.step(dt);
}

void City::load_map(const MapConfig& cfg) {
    physics_world.load_static_geometry(cfg);
    spawns_by_route.clear();
    for (const auto& s : cfg.spawns) {
        spawns_by_route[s.race_id].push_back(s);
    }
    
    for (auto& kv : spawns_by_route) {
        auto& vec = kv.second;
        std::sort(vec.begin(), vec.end(), [](const SpawnPoint& a, const SpawnPoint& b) {
            return a.car_id < b.car_id;
        });
    }
    checkpoints_by_route = cfg.checkpoints;
    
}

Track City::build_track(const std::string& route_id) const {
    Track t;
    t.route_id = route_id;

    auto it = checkpoints_by_route.find(route_id);
    if (it != checkpoints_by_route.end()) {
        t.checkpoints = it->second;
        t.checkpoint_count = (uint32_t)(t.checkpoints.size());
    } else {
        t.checkpoint_count = 0;
    }

    return t;
}

SpawnPoint City::get_spawn_for_index(size_t index, const std::string& route_id) const {
    const std::vector<SpawnPoint>* vec = nullptr;
    
    auto it = spawns_by_route.find(route_id);
    if (it != spawns_by_route.end() && !it->second.empty()) {
        vec = &it->second;
    } else {
        for (const auto& kv : spawns_by_route) {
            if (!kv.second.empty()) {
                vec = &kv.second;
                break;
            }
        }
    }

    const size_t spawn_idx = index % vec->size();
    const auto& sp = (*vec)[spawn_idx];

    return sp;
}

const std::vector<Checkpoint>& City::get_checkpoints_for_route(const std::string& route_id) const {
    auto it = checkpoints_by_route.find(route_id);
    if (it != checkpoints_by_route.end()) {
        return it->second;
    }
    static const std::vector<Checkpoint> empty_checkpoints;
    return empty_checkpoints;
}
