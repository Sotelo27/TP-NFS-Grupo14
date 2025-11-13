#include "map_config_loader.h"

#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>

MapConfig MapConfigLoader::load_tiled_file(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);
    if (!root) {
        throw std::runtime_error("MapConfigLoader: cannot load " + path);
    }

    MapConfig cfg;

    // pixels_per_meter con fallback
    if (root["pixels_per_meter"]) {
        cfg.pixels_per_meter = root["pixels_per_meter"].as<float>();
    } else if (root["tilewidth"]) {
        cfg.pixels_per_meter = root["tilewidth"].as<float>();
    } else {
        cfg.pixels_per_meter = 32.f;
    }

    if (!root["layers"] || !root["layers"].IsSequence()) {
        return cfg;
    }

    const std::string collisions_name   = "Colisiones";
    const std::string spawns_prefix     = "Spawns_";
    const std::string checkpoints_prefix = "Checkpoint_";

    for (const auto& layer : root["layers"]) {
        const std::string ltype = layer["type"] ? layer["type"].as<std::string>() : std::string();
        const std::string lname = layer["name"] ? layer["name"].as<std::string>() : std::string();
        if (ltype != "objectgroup" || !layer["objects"]) continue;

        const bool is_collision_layer   = (lname == collisions_name);
        const bool is_spawns_layer      = (lname.rfind(spawns_prefix, 0) == 0);
        const bool is_checkpoints_layer = (lname.rfind(checkpoints_prefix, 0) == 0);

        // Deducir race_id a partir del nombre de la capa (Spawns_A → "A", Checkpoint_B → "B")
        std::string spawns_race_id;
        std::string checkpoints_race_id;

        if (is_spawns_layer) {
            spawns_race_id = lname.substr(spawns_prefix.size());
            if (spawns_race_id.empty()) spawns_race_id = "A";
        }
        if (is_checkpoints_layer) {
            checkpoints_race_id = lname.substr(checkpoints_prefix.size());
            if (checkpoints_race_id.empty()) checkpoints_race_id = "A";
        }

        for (const auto& obj : layer["objects"]) {
            const float ox = obj["x"].as<float>();
            const float oy = obj["y"].as<float>();

            // ---------------- SPAWNS ----------------
            if (is_spawns_layer) {
                bool is_point = obj["point"] ? obj["point"].as<bool>() : false;
                if (is_point || (!obj["width"] && !obj["height"])) {
                    SpawnPoint s{};
                    s.x_px = ox;
                    s.y_px = oy;
                    s.angle_deg = obj["rotation"] ? obj["rotation"].as<float>() : 0.f;
                    s.id = obj["id"] ? obj["id"].as<int>() : -1;

                    s.car_id  = -1;
                    s.race_id = spawns_race_id; // default por capa

                    if (obj["properties"]) {
                        for (const auto& p : obj["properties"]) {
                            const std::string pname = p["name"].as<std::string>();
                            if (pname == "id_car") {
                                s.car_id = p["value"].as<int>();
                            } else if (pname == "race_id") {
                                s.race_id = p["value"].as<std::string>();
                            }
                        }
                    }

                    cfg.spawns.push_back(s);
                }
                continue;
            }

            // ---------------- CHECKPOINTS ----------------
            if (is_checkpoints_layer) {
                Checkpoint cp{};
                cp.x_px = ox;
                cp.y_px = oy;
                cp.w_px = obj["width"]  ? obj["width"].as<float>()  : 0.f;
                cp.h_px = obj["height"] ? obj["height"].as<float>() : 0.f;
                cp.rotation_deg = obj["rotation"] ? obj["rotation"].as<float>() : 0.f;

                cp.race_id = checkpoints_race_id; // default por capa

                if (obj["properties"]) {
                    for (const auto& p : obj["properties"]) {
                        const std::string pname = p["name"].as<std::string>();
                        if (pname == "index") {
                            cp.index = p["value"].as<int>();
                        } else if (pname == "type") {
                            cp.type = p["value"].as<std::string>();
                        } else if (pname == "race_id") {
                            cp.race_id = p["value"].as<std::string>();
                        }
                    }
                }

                cfg.checkpoints[cp.race_id].push_back(std::move(cp));
                continue;
            }

            // ---------------- COLISIONES ----------------
            if (!is_collision_layer) continue;

            if (obj["polyline"]) {
                PolylineCollider pl{};
                pl.x_px = 0.f;
                pl.y_px = 0.f;
                pl.loop = false;

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

            const float w = obj["width"]  ? obj["width"].as<float>()  : 0.f;
            const float h = obj["height"] ? obj["height"].as<float>() : 0.f;
            if (w > 0.f && h > 0.f) {
                RectCollider r{};
                r.x_px = ox;
                r.y_px = oy;
                r.w_px = w;
                r.h_px = h;
                r.rotation_deg = obj["rotation"] ? obj["rotation"].as<float>() : 0.f;

                // Opcional: leer props de colisión
                if (obj["properties"]) {
                    for (const auto& p : obj["properties"]) {
                        const std::string pname = p["name"].as<std::string>();
                        if (pname == "friction") {
                            r.friction = p["value"].as<float>();
                        } else if (pname == "restitution") {
                            r.restitution = p["value"].as<float>();
                        } else if (pname == "is_sensor") {
                            r.is_sensor = p["value"].as<bool>();
                        }
                    }
                }

                cfg.rects.push_back(r);
            }
        }
    }

    // Ordenar checkpoints por índice en cada carrera
    for (auto& kv : cfg.checkpoints) {
        auto& vec = kv.second;
        std::sort(vec.begin(), vec.end(),
                  [](const Checkpoint& a, const Checkpoint& b) {
                      return a.index < b.index;
                  });
        std::cout << "[MapLoader] race '" << kv.first
                  << "' total checkpoints=" << vec.size() << "\n";
    }

    // === LOG RESUMEN FINAL ===
    std::cout << "[MapLoader] =====================\n";
    std::cout << "[MapLoader] Carga completa del mapa\n";
    std::cout << "[MapLoader] RectCollider : " << cfg.rects.size() << "\n";
    std::cout << "[MapLoader] Polyline     : " << cfg.polylines.size() << "\n";
    std::cout << "[MapLoader] Spawns       : " << cfg.spawns.size() << "\n";
    
    std::size_t total_checkpoints = 0;
    for (const auto& kv : cfg.checkpoints) {
        total_checkpoints += kv.second.size();
    }
    std::cout << "[MapLoader] Checkpoints  : " << total_checkpoints << "\n";
    std::cout << "[MapLoader] =====================\n";

    return cfg;
}
