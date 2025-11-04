#ifndef RACE_H
#define RACE_H

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <vector>

#include "../common/player_aux.h"
#include "physics_world.h"
#include "race_participant.h"
#include "../common/dto/input_state.h"
#include "../common/car_model.h"
#include "Player/car.h"

class Race {
private:
    uint32_t id;
    PhysicsWorld& physics;
    std::unordered_map<size_t, RaceParticipant> parts;
    std::unordered_map<size_t, std::unique_ptr<Car>> cars;

    /*
     * Devuelve la direccion de aceleración del jugador segun su input
     *   +1 : acelerar
     *   -1 : retroceder
     *    0 : sin movimiento
     */
    static float resolve_acceleration_input(const InputState& input);

    /*
     * Devuelve la direccion de giro del jugador segun su input digital:
     *   +1 : girar a la derecha
     *   -1 : girar a la izquierda
     *    0 : recto
     */
    static float resolve_rotation_input(const InputState& input);

    /*
    * Aplica al auto del jugador(body) una fuerza hacia adelante o hacia atras
    * segun la direccion de aceleracion obtenida del input y las
    * caracteristicas fisicas del auto (CarModel).
     */
    void apply_acceleration_force(size_t player_id, float throttle, const CarModel& car);

public:
    Race(uint32_t id, PhysicsWorld& external_world);

    /*
    * Agrega un jugador a la carrera con su CarModel y posicioon inicial
     * Tambien crea su cuerpo fisico en el mundo de Box2D
     */
    void add_player(size_t playerId, const CarModel& spec, float spawnX_px, float spawnY_px);
    
    
    /*
     * Marca al jugador como desconectado y destruye su body fisico removiendolo de la carrera
     */
    void remove_player(size_t playerId);

    /*
    * Aplica el InputState del jugador: acelera, gira segun CarModel
     * y limita la velocidad maxima cuando corresponde
     */
    void apply_input(size_t playerId, const InputState& input);

    /*
     * Construye el snapshot con las posiciones (x,y) y angulos actuales de todos los
     * participantes activos/terminados, para ser enviado a los clientes.
     */
    std::vector<PlayerPos> snapshot_poses() const;
};

#endif
