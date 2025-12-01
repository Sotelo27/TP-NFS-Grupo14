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
    map_cfg = cfg;
    routes = cfg.routes; // ASEGURAR QUE SE ASIGNAN LAS RUTAS
    
    std::cout << "[City] Loaded map with:\n";
    std::cout << "  - " << routes.size() << " routes\n";
    std::cout << "  - " << map_cfg.npc_spawns.size() << " NPC spawns\n";
    
    for (const auto& route : routes) {
        std::cout << "  - Route '" << route.route_id << "': " 
                  << route.waypoints.size() << " waypoints\n";
    }
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

const std::vector<NpcSpawn>& City::get_npc_spawns() const {
    return map_cfg.npc_spawns;
}

const std::vector<Route>& City::get_routes() const {
    return routes;
}

const Route* City::find_closest_route(float x_px, float y_px) const {
    if (routes.empty()) return nullptr;
    
    const Route* closest = nullptr;
    float min_dist = 1e9f;
    
    for (const auto& route : routes) {
        if (route.waypoints.empty()) continue;
        
        // Distancia al primer waypoint de la ruta
        const auto& wp = route.waypoints[0];
        float dx = wp.x_px - x_px;
        float dy = wp.y_px - y_px;
        float dist = std::sqrt(dx*dx + dy*dy);
        
        if (dist < min_dist) {
            min_dist = dist;
            closest = &route;
        }
    }
    
    return closest;
}

const Waypoint* City::find_closest_waypoint_in_route(const std::string& route_id, float x_px, float y_px) const {
    for (const auto& route : routes) {
        if (route.route_id != route_id) continue;
        if (route.waypoints.empty()) return nullptr;
        
        const Waypoint* closest = nullptr;
        float min_dist = 1e9f;
        
        for (const auto& wp : route.waypoints) {
            float dx = wp.x_px - x_px;
            float dy = wp.y_px - y_px;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if (dist < min_dist) {
                min_dist = dist;
                closest = &wp;
            }
        }
        
        return closest;
    }
    
    return nullptr;
}
