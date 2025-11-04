#ifndef MAP_CONFIG_LOADER_H
#define MAP_CONFIG_LOADER_H

#include <string>
#include "../../common/dto/map_config.h"

class MapConfigLoader {
public:
    static MapConfig load_tiled_file(const std::string& path,
                                     const std::string& collisions_layer = "Colisiones",
                                     const std::string& spawns_layer = "Spawns");
};

#endif
