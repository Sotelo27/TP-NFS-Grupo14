#ifndef RACE_H
#define RACE_H

#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

#include "../../common/player_aux.h"
#include "../../common/dto/map_tick_info.h"
#include "../../common/dto/input_state.h"
#include "../../common/car_model.h"
#include "../physics_world.h"
#include "race_participant.h"
#include "../Player/car.h"

class Race {
private:
    uint32_t id;
    float race_duration{0.f};
    bool is_finished_{false};
    PhysicsWorld& physics;
    std::unordered_map<size_t, RaceParticipant> parts;
    std::unordered_map<size_t, std::unique_ptr<Car>> cars;
    Track track;

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

public:
    Race(uint32_t id, PhysicsWorld& external_world);

    /*
     * Agrega un jugador a la carrera con su CarModel y posicioon inicial
     * Tambien crea su cuerpo fisico en el mundo de Box2D
     */
    void add_player(size_t playerId, const CarModel& spec, uint8_t car_id, float spawnX_px, float spawnY_px);
    
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
     * Establece el track de la carrera
     */
    void set_track(const Track& new_track);

    /*
     * Obtiene el ID de la ruta del track actual  
     */
    const std::string& get_route_id() const;

    /*
     * Avanza el tiempo de la carrera en dt segundos
     */
    void advance_time(float dt);

    /*
     * Maneja la logica cuando un auto cruza un checkpoint
     */
    void on_car_checkpoint(const std::string& race_id, size_t player_id, uint32_t checkpoint_id);   
    /*
     * Devuelve el tiempo transcurrido de la carrera en segundos
     */
    uint32_t get_race_time_seconds() const;

    bool is_finished() const noexcept;

    /*
     * Construye el snapshot con las posiciones (x,y) y angulos actuales de todos los
     * participantes activos/terminados, para ser enviado a los clientes.
     */
    std::vector<PlayerPos> snapshot_poses() const;

    /*
     * Construye el snapshot con la informacion ( posiciones y vida) de tick de todos los
     * participantes activos/terminados, para ser enviado a los clientes
     */
    std::vector<PlayerTickInfo> snapshot_ticks() const;

    /*
     * Compara dos jugadores segun su RankInfo y determina si el primero es mejor que el segundo
     */
    static bool compare_rank(const RankInfo& a, const RankInfo& b);
    
    /*
     * Calcula y asigna las posiciones en la carrera de cada jugador
     */
    void calculate_ranking_positions(std::vector<PlayerTickInfo>& ticks, std::vector<RankInfo>& ranking) const;
};

#endif
