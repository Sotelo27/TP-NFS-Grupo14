#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include <cstdint>
#include <unordered_map>
#include <utility>

#include <box2d/box2d.h>

#include "../common/car_model.h"
#include "../common/pose.h"


class PhysicsWorld {
private:
    b2World world;
    std::unordered_map<size_t, b2Body*> bodies;

    /*
     * Conversion de unidades ----
     * - Internamente trabajamos en metros (Box2D)
     * - Para el cliente usamos unidades mas finas (128 por metro) para suavizar el movimiento
     * - Al enviar snapshots, convertimos de metros a unidades
     */
    static inline float toMeters(int16_t units) { return static_cast<float>(units); }
    static inline int16_t toUnits(float meters) {
        constexpr float UNITS_PER_METER = 128.0f;
        return static_cast<int16_t>(meters * UNITS_PER_METER);
    }

public:
    PhysicsWorld();

    /*
     * Crea el cuerpo fisico del jugador en su posicion inicial,
     * usando el modelo de auto (masa, velocidad y demas prarmetros)
     */
    void create_body_with_spec(size_t id, int16_t x_units, int16_t y_units, const CarModel& spec);

    /*
     * Elimina el cuerpo del jugador del mundo Box2d
     */
    void destroy_body(size_t id);

    /*
     * Avanza la simulacion en dt segundos
     */
    void step(float dt);

    /*
     * Empuja al auto aplicando una fuerza en su centro
     */
    void apply_force_center(size_t id, float fx, float fy);

    /*
     * Hace girar al auto aplicando un torque
     */
    void apply_torque(size_t id, float torque);

    /*
     * Devuelve la rotacion actual en radianes
     */
    float get_angle(size_t id) const;

    /*
     * Obtiene la posicion del body completa (posicion + angulo)
     */
    Pose get_pose(size_t id) const;

    /*
     * Pone un tope a la velocidad lineal (m/s).
     */
    void cap_linear_speed(size_t id, float max_mps);

    /*
     * Devuelve la velocidad lineal actual (m/s).
     */
    float get_linear_speed(size_t id) const;
};

#endif
