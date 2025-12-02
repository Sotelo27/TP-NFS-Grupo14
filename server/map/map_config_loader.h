#ifndef MAP_CONFIG_LOADER_H
#define MAP_CONFIG_LOADER_H

#include <string>
#include "../../common/dto/map_config.h"
#include <yaml-cpp/yaml.h>

class MapConfigLoader {
public:
    static MapConfig load_from_yaml(const YAML::Node& root);
};

#endif