#ifndef CITY_H
#define CITY_H

#include <vector>
#include "physics_world.h"
#include "../common/dto/map_config.h"

class City {
private:
    PhysicsWorld physics_world;
    std::vector<SpawnPoint> spawns;

public:
    City();

    /*
     * Obtiene referencia al mundo físico
     */
    PhysicsWorld& get_world();

    /*
     * Avanza la simulación física
     */
    void step(float dt);

    /*
     * Carga la geometría del mapa
     */
    void load_map(const MapConfig& cfg);

    /*
     * Obtiene un spawn point según el índice
     * Si no hay spawns, retorna una posición por defecto
     */
    SpawnPoint get_spawn_for_index(size_t index) const;

    /*
     * Configura los puntos de spawn
     */
    void set_spawns(const std::vector<SpawnPoint>& new_spawns);
};

#endif
