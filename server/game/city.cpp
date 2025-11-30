#include "city.h"
#include <algorithm>
#include <iostream>
#include <random>
#include "../../common/dto/map_config.h"

// Función utilitaria solo visible en este archivo
static bool is_point_in_rect(float x, float y, const RectCollider& rect) {
    // Ignora rotación para simplificar (rectángulos axis-aligned)
    return x >= rect.x_px && x <= rect.x_px + rect.w_px &&
           y >= rect.y_px && y <= rect.y_px + rect.h_px;
}

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

    // Filtrar spawns de jugadores que estén fuera de colisiones
    auto is_valid_spawn = [&](const SpawnPoint& s) {
        for (const auto& rect : cfg.rects) {
            if (is_point_in_rect(s.x_px, s.y_px, rect)) {
                return false;
            }
        }
        return true;
    };

    for (const auto& s : cfg.spawns) {
        if (is_valid_spawn(s)) {
            spawns_by_route[s.race_id].push_back(s);
        }
    }

    for (auto& kv : spawns_by_route) {
        auto& vec = kv.second;
        std::sort(vec.begin(), vec.end(), [](const SpawnPoint& a, const SpawnPoint& b) {
            return a.car_id < b.car_id;
        });
    }
    checkpoints_by_route = cfg.checkpoints;
    map_cfg = cfg; // Guardar para generación procedural
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

std::vector<std::string> City::get_route_ids() const {
    std::vector<std::string> ids;
    ids.reserve(checkpoints_by_route.size());

    for (const auto& kv : checkpoints_by_route) {
        ids.push_back(kv.first);
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

// Devuelve true si el punto (x, y) está libre de colisiones (en píxeles)
bool City::is_point_free(float x_px, float y_px, float margin) const {
    for (const auto& rect : map_cfg.rects) {
        if (x_px >= rect.x_px - margin && x_px <= rect.x_px + rect.w_px + margin &&
            y_px >= rect.y_px - margin && y_px <= rect.y_px + rect.h_px + margin) {
            return false;
        }
    }
    return true;
}

// Genera N puntos de spawn libres de colisión para NPCs
std::vector<SpawnPoint> City::generate_npc_spawns(size_t count) const {
    std::vector<SpawnPoint> spawns;
    if (map_cfg.rects.empty()) return spawns;

    // Determinar límites del mapa
    float min_x = 0, min_y = 0, max_x = 4000, max_y = 4000;
    for (const auto& rect : map_cfg.rects) {
        if (rect.x_px + rect.w_px > max_x) max_x = rect.x_px + rect.w_px;
        if (rect.y_px + rect.h_px > max_y) max_y = rect.y_px + rect.h_px;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_x(min_x, max_x);
    std::uniform_real_distribution<float> dist_y(min_y, max_y);

    size_t max_attempts = count * 20;
    size_t attempts = 0;
    while (spawns.size() < count && attempts < max_attempts) {
        float x = dist_x(gen);
        float y = dist_y(gen);
        if (is_point_free(x, y, 30.0f)) {
            spawns.push_back(SpawnPoint{x, y, 0.f, -1, -1, "NPC"});
        }
        ++attempts;
    }
    return spawns;
}
