#ifndef MAP_CONFIG_H
#define MAP_CONFIG_H

#include <cstdint>
#include <utility>
#include <vector>

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
};

struct MapConfig {
    std::vector<RectCollider> rects;
    std::vector<PolylineCollider> polylines;
    std::vector<SpawnPoint> spawns;
    float pixels_per_meter{32.f};
};

#endif
