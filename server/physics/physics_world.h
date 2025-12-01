#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <memory>
#include <vector>

#include <box2d/box2d.h>

#include "../../common/car_model.h"
#include "../../common/dto/pose.h"
#include "../../common/dto/map_config.h"
#include "building_entity.h"
#include "border_entity.h"
#include "checkpoint_entity.h"
#include "contact_listener.h"
#include "checkpoint_event.h"
#include "damage_event.h"

class PhysicsWorld {
private:
    b2World world;
    std::unordered_map<size_t, b2Body*> bodies;
    std::vector<b2Body*> static_bodies; // walls, boundaries, checkpoints
    std::vector<std::unique_ptr<Entidad>> static_entities;
    size_t next_static_id_{1};

    /*
     * Listener global de colisiones para el mundo Box2D
     * de PhysicsWorld. Su responsabilidad actual:
     *   - Car vs Car: aplica daño base a ambos
     *   - Car vs (Building|Border): aplica daño al Car
     *   - Car vs Checkpoint: registra evento de cruce de checkpoint
     */
    ContactListener contact_listener;

    /*
     * Crea un rectangulo que simula el edificio a partir de medidas en pixeles
     * pixel_to_meters = 1 / PPM  (factor para convertir px en metros)
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
     * Crea un cuerpo eststico sensor para un checkpoint rectangular
     */
    void add_checkpoint_body_px(const Checkpoint& cp, float invPPM);

public:
    PhysicsWorld();

    /*
     * Crea el cuerpo fisico del jugador en su posicion inicial,
     * usando el modelo de auto (masa, velocidad y demas prarmetros)
     */
    // Crea el body físico de un auto según su CarModel en la posición inicial (pos en metros)
    void create_car_body(size_t id, float x_meters, float y_meters, const CarModel& spec);

    /*
     * Elimina el cuerpo del jugador del mundo Box2d
     */
    void destroy_body(size_t id);

    /*
     * Avanza la simulacion en dt segundos
     */
    void step(float dt);

    /*
     * Obtiene el puntero al body de Box2D asociado a un id
     * Retorna nullptr si no existe
     */
    b2Body* get_body(size_t id) const;

    /*
     * Carga geotrica estatica (paredes, bordes) al mundo fisico desde MapConfig
     * Convierte de pixeles a metros usando cfg.pixels_per_meter
     */
    void load_static_geometry(const MapConfig& cfg);

    /*
     * Obtiene todos los eventos de cruce de checkpoint registrados
     */
    std::vector<CheckpointEvent> consume_checkpoint_events();

    /*
     *  Obtiene todos los eventos de daño registrados
     */
    std::vector<DamageEvent> consume_damage_events();

    /*
     * Limpia toda la geometria estatica previamente cargada
     */
    void clear_static_geometry();
};

#endif
