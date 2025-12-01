#ifndef CITY_H
#define CITY_H

#include <vector>
#include "../physics_world.h"
#include "../../common/dto/map_config.h"

class City {
private:
    PhysicsWorld physics_world;
    std::unordered_map<std::string, std::vector<Checkpoint>> checkpoints_by_route; 
    std::unordered_map<std::string, std::vector<SpawnPoint>> spawns_by_route;
    MapConfig map_cfg; 
    std::vector<Route> routes;

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
    SpawnPoint get_spawn_for_index(size_t index,const std::string& route_id) const;

    /*
     * Construye el track de la ciudad para una ruta específica
     */
    Track build_track(const std::string& route_id) const;

    /*
     * Obtiene los checkpoints para una ruta especifica
    */
    const std::vector<Checkpoint>& get_checkpoints_for_route(const std::string& route_id) const;

    /*
     * Obtiene los ids de las rutas disponibles
    */
    std::vector<std::string> get_route_ids() const;

    /*
     * Obtiene los puntos de spawn de NPCs
    */
    const std::vector<NpcSpawn>& get_npc_spawns() const;

    const std::vector<Route>& get_routes() const;
    const Route* find_closest_route(float x_px, float y_px) const;
    const Waypoint* find_closest_waypoint_in_route(const std::string& route_id, float x_px, float y_px) const;
};

#endif
