#pragma once
#include <string>
#include <yaml-cpp/yaml.h>

class ServerConfig {
public:
    float nitro_duracion = 3.0f;
    uint8_t max_players = 8;
    uint32_t tiempo_partida = 300; // nuevo: tiempo de partida en segundos

    explicit ServerConfig(const std::string& path) {
        YAML::Node config = YAML::LoadFile(path);
        if (config["nitro_duracion"])
            nitro_duracion = config["nitro_duracion"].as<float>();
        if (config["max_players"])
            max_players = config["max_players"].as<uint8_t>();
        if (config["tiempo_partida"])
            tiempo_partida = config["tiempo_partida"].as<uint32_t>();
    }
    // getters para acceso global
    float get_nitro_duracion() const { return nitro_duracion; }
    uint8_t get_max_players() const { return max_players; }
    uint32_t get_tiempo_partida() const { return tiempo_partida; }
};
