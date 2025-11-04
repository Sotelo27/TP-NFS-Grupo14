#ifndef CITY_H
#define CITY_H

#include "physics_world.h"
#include "../common/dto/map_config.h"

class City {
private:
    PhysicsWorld world;
    std::vector<SpawnPoint> spawns_cfg;

public:
    City() = default;

    void load_map(const MapConfig& cfg) {
        world.load_static_geometry(cfg);
        spawns_cfg.clear();
        spawns_cfg.reserve(cfg.spawns.size());
        for (const auto& sp : cfg.spawns) {
            spawns_cfg.push_back(sp);
        }
    }

    SpawnPoint get_spawn_for_index(size_t idx) const {
        if (spawns_cfg.empty()) return SpawnPoint{};
        return spawns_cfg[idx % spawns_cfg.size()];
    }

    void step(float dtSeconds) { world.step(dtSeconds); }

    PhysicsWorld& get_world() { return world; }
    const PhysicsWorld& get_world() const { return world; }
};

#endif
