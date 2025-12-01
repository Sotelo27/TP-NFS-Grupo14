#ifndef MAP_CONFIG_H
#define MAP_CONFIG_H

#include <cstdint>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>

struct RectCollider {
    float x_px{0.f};
    float y_px{0.f};
    float w_px{0.f};
    float h_px{0.f};
    float rotation_deg{0.f};
    float friction{0.9f};
    float restitution{0.0f};
    bool is_sensor{false};
};

struct PolylineCollider {
    float x_px{0.f};
    float y_px{0.f};
    std::vector<std::pair<float, float>> points_px;
    bool loop{false};
    float friction{0.9f};
    float restitution{0.0f};
    bool is_sensor{false};
};

struct SpawnPoint {
    float x_px{0.f};
    float y_px{0.f};
    float angle_deg{0.f};
    int   id{-1};
    int   car_id{-1};
    std::string race_id{"A"};
};
struct Checkpoint {
    float x_px{0.f};
    float y_px{0.f};
    float w_px{0.f};
    float h_px{0.f};
    float rotation_deg{0.f};
    int   index{0};
    std::string type{"normal"};
    std::string race_id{"A"};
    bool  is_sensor{true};
};

struct NpcSpawn {
    float x_px{0.f};
    float y_px{0.f};
    float angle_deg{0.f};
};


struct Track {
    std::string route_id;
    std::vector<Checkpoint> checkpoints;
    uint32_t checkpoint_count;
};

struct Waypoint {
    std::string route_id;
    uint32_t index;
    float x_px;
    float y_px;
};

struct Route {
    std::string route_id;
    std::vector<Waypoint> waypoints;
};

struct MapConfig {
    std::vector<RectCollider> rects;
    std::vector<PolylineCollider> polylines;
    std::vector<SpawnPoint> spawns;
    std::unordered_map<std::string, std::vector<Checkpoint>> checkpoints;
    std::vector<NpcSpawn> npc_spawns; 
    float pixels_per_meter{32.f};
    std::vector<Route> routes;
};

#endif
