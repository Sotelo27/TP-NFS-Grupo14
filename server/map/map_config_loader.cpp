#include "map_config_loader.h"
#include <yaml-cpp/yaml.h>
#include <stdexcept>

MapConfig MapConfigLoader::load_tiled_file(const std::string& path,
                                           const std::string& collisions_layer,
                                           const std::string& spawns_layer) {
    YAML::Node root = YAML::LoadFile(path);
    if (!root) throw std::runtime_error("MapConfigLoader: cannot load " + path);

    MapConfig cfg;
    cfg.pixels_per_meter = root["pixels_per_meter"]
        ? root["pixels_per_meter"].as<float>()
        : (root["tilewidth"] ? root["tilewidth"].as<float>() : 32.f);

    if (!root["layers"] || !root["layers"].IsSequence()) {
        return cfg;
    }

    for (const auto& layer : root["layers"]) {
        const std::string ltype = layer["type"] ? layer["type"].as<std::string>() : std::string();
        const std::string lname = layer["name"] ? layer["name"].as<std::string>() : std::string();
        if (ltype != "objectgroup" || !layer["objects"]) continue;

        const bool is_collision = (lname == collisions_layer);
        const bool is_spawns    = (lname == spawns_layer);

        for (const auto& obj : layer["objects"]) {
            const float ox = obj["x"].as<float>();
            const float oy = obj["y"].as<float>();

            if (is_spawns) {
                bool is_point = obj["point"] ? obj["point"].as<bool>() : false;
                if (is_point || (!obj["width"] && !obj["height"])) {
                    SpawnPoint s{};
                    s.x_px = ox;
                    s.y_px = oy;
                    s.angle_deg = obj["rotation"] ? obj["rotation"].as<float>() : 0.f;
                    s.id = obj["id"] ? obj["id"].as<int>() : -1;
                    cfg.spawns.push_back(s);
                }
                continue;
            }

            if (!is_collision) continue;

            if (obj["polyline"]) {
                PolylineCollider pl{};
                pl.x_px = 0.f; pl.y_px = 0.f; pl.loop = false;
                for (const auto& p : obj["polyline"]) {
                    float px = 0.f, py = 0.f;
                    if (p.IsMap()) {
                        px = p["x"].as<float>();
                        py = p["y"].as<float>();
                    } else if (p.IsSequence() && p.size() >= 2) {
                        px = p[0].as<float>();
                        py = p[1].as<float>();
                    }
                    pl.points_px.emplace_back(ox + px, oy + py);
                }
                cfg.polylines.push_back(std::move(pl));
                continue;
            }

            const float w  = obj["width"]  ? obj["width"].as<float>()  : 0.f;
            const float h  = obj["height"] ? obj["height"].as<float>() : 0.f;
            if (w > 0.f && h > 0.f) {
                RectCollider r{};
                r.x_px = ox; r.y_px = oy; r.w_px = w; r.h_px = h;
                r.rotation_deg = obj["rotation"] ? obj["rotation"].as<float>() : 0.f;
                cfg.rects.push_back(r);
            }
        }
    }

    return cfg;
}
