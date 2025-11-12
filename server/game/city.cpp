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

void City::set_spawns(const std::vector<SpawnPoint>& new_spawns) {
    spawns = new_spawns;
}
