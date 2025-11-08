#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <memory>
#include <vector>

#include <box2d/box2d.h>

#include "../common/car_model.h"
#include "../common/dto/pose.h"
#include "../common/dto/map_config.h"
#include "physics/building_entity.h"
#include "physics/border_entity.h"


class PhysicsWorld {
private:
    b2World world;
    std::unordered_map<size_t, b2Body*> bodies;
    std::vector<b2Body*> static_bodies; // walls, boundaries, checkpoints
    
    std::vector<std::unique_ptr<Entidad>> static_entities;
    size_t next_static_id_{1};

    // Ajuste/tolerancia para colisionadores rectangulares: reducimos 2px por lado
    // para evitar contactos "tempranos" respecto al arte de los edificios.
    static constexpr float rect_collider_margin_px = 32.0f;

    /*
     * Conversion de unidades ----
     * - Internamente trabajamos en metros (Box2D)
     * - Esta clase expone Pose en "units" (128 u/m) por compatibilidad con el servidor
     * - La capa superior es responsable de convertir esas units a pixeles al enviar al cliente
     */
    static inline float toMeters(int16_t units) {
        constexpr float UNITS_PER_METER = 128.0f;
        return static_cast<float>(units) / UNITS_PER_METER;
    }
    static inline int16_t toUnits(float meters) {
        constexpr float UNITS_PER_METER = 128.0f;
        return static_cast<int16_t>(meters * UNITS_PER_METER);
    }

    /*
     * Crea un cuerpo estático rectangular a partir de un RectCollider (coordenadas en pixeles)
     * - Aplica rect_collider_margin_px para un debug visual
     */
    void add_static_rect_body_px(const RectCollider& r, float invPPM);

    /*
     * Crea fixtures estáticos a partir de una polyline (coordenadas en pixeles).
     * - Si loop==true, crea un loop (closed chain)
     * - Si loop==false, crea dos open chains (ida y vuelta) usando los extremos como
     *   ghost vertices para colision bidireccional y evitar bordes "de un solo lado"
     */
    void add_static_polyline_bodies_px(const PolylineCollider& pl, float invPPM);

    /*
     * Convierte los puntos de una polyline desde pixeles (locales al objeto) a metros (globales),
     * aplicando el offset (x_px, y_px) del objeto.
     */
    std::vector<b2Vec2> make_polyline_vertices_m(const PolylineCollider& pl, float invPPM) const;

    /*
     * Calcula ghost vertices para una lista de vertices (en metros)
     * Implementación actual: usa los extremos (sin extrapolación) para simplicidad
     * Útil para cadenas abiertas en Box2D (b2ChainShape::CreateChain)
     */
    void compute_ghost_vertices(const std::vector<b2Vec2>& verts, b2Vec2& ghostA, b2Vec2& ghostB) const;

public:
    PhysicsWorld();

    /*
     * Crea el cuerpo fisico del jugador en su posicion inicial,
     * usando el modelo de auto (masa, velocidad y demas prarmetros)
     */
    // Crea el body físico de un auto según su CarModel en la posición inicial
    void create_car_body(size_t id, int16_t x_units, int16_t y_units, const CarModel& spec);

    /*
     * Elimina el cuerpo del jugador del mundo Box2d
     */
    void destroy_body(size_t id);

    /*
     * Avanza la simulacion en dt segundos
     */
    void step(float dt);

    /*
     * Obtiene la posicion del body completa (posicion + angulo)
     */
    Pose get_pose(size_t id) const;

    // Acceso controlado al body (solo lectura del puntero)
    b2Body* get_body(size_t id) const {
        auto it = bodies.find(id);
        if (it == bodies.end()) return nullptr;
        return it->second;
    }

    /*
     * Carga geotrica estatica (paredes, bordes) al mundo fisico desde MapConfig
     * Convierte de pixeles a metros usando cfg.pixels_per_meter
     */
    void load_static_geometry(const MapConfig& cfg);

    /*
     * Limpia toda la geometria estatica previamente cargada
     */
    void clear_static_geometry();
};

#endif
