#include "city.h"

City::City() : physics_world() {}

PhysicsWorld& City::get_world() {
    return physics_world;
}

void City::step(float dt) {
    physics_world.step(dt);
}

void City::load_map(const MapConfig& cfg) {
    physics_world.load_static_geometry(cfg);
    spawns = cfg.spawns;
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

SpawnPoint City::get_spawn_for_index(size_t index) const {
    // Sirve para retornar posicion por defecto
    if (spawns.empty()) {
        SpawnPoint default_spawn;
        default_spawn.x_px = 400.0f;
        default_spawn.y_px = 300.0f;
        default_spawn.angle_deg = 0.0f;
        default_spawn.id = 0;
        return default_spawn;
    }
    
    // Usar módulo para ciclar entre los spawns disponibles
    size_t spawn_idx = index % spawns.size();
    return spawns[spawn_idx];
}

const std::vector<Checkpoint>& City::get_checkpoints_for_route(const std::string& route_id) const {
    auto it = checkpoints_by_route.find(route_id);
    if (it != checkpoints_by_route.end()) {
        return it->second;
    }
    static const std::vector<Checkpoint> empty_checkpoints;
    return empty_checkpoints;
}

void City::set_spawns(const std::vector<SpawnPoint>& new_spawns) {
    spawns = new_spawns;
}
