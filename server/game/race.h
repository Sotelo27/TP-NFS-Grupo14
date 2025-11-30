#ifndef RACE_H
#define RACE_H

#include <cstdint>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>
#include <map>

#include "../../common/player_aux.h"
#include "../../common/dto/map_tick_info.h"
#include "../../common/dto/input_state.h"
#include "../../common/car_model.h"
#include "../physics_world.h"
#include "race_info.h"
#include "../Player/car.h"
#include "../Player/player.h"
#include "../../common/constants.h"

class Race {
private:
    uint32_t id;
    float race_duration{0.f};
    RaceState state_{RaceState::Running};
    PhysicsWorld& physics;
    std::unordered_map<size_t, RaceParticipant> parts;
    std::unordered_map<size_t, std::unique_ptr<Car>> cars;
    Track track;

    // Mapa de player_id a Car*
    std::map<size_t, Car*> cars_by_player;
    std::map<size_t, Player*> players_by_id; // NUEVO: para asociar Player*

    /*
     * Verifica los estados de vida de los jugadores y los descalifica si no tienen vida
     */
    void check_health_states();


    /*
     * Verifica si se ha excedido el tiempo maximo de la carrera 
     */
    void check_time_limit();

    /*
     * Evalua si debe darse por finalizada la carrera: cuando ya no quedan
     * participantes en estado Active
     */
    void evaluate_finish();

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
    void add_player(size_t player_id, const CarModel& spec, uint8_t car_id, float spawnX_px, float spawnY_px, Player* player_ptr);
    
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
     * Aplica un cheat al jugador
     */
    void apply_cheat(size_t playerId, uint8_t cheat_code);

    // NUEVO: Teletransporta el auto al siguiente checkpoint
    void teleport_to_next_checkpoint(size_t playerId);

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

    /*
     * Indica si la carrera ha finalizado
     */
    bool is_finished() const noexcept;

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
    
    /*
     * Construye el resultado de la carrera: para cada jugador incluye su estado final
     * y el tiempo en segundos en que termino
     */
    RaceResult build_race_results() const;

    /*
     * Destruye todos los cuerpos de autos en el PhysicsWorld y limpia las estructuras
     * internas (cars y parts).
     */
    void clear_cars();

    /*
     * Establece el puntero del jugador
     */
    void set_player_ptr(size_t player_id, Player* player_ptr); // NUEVO
};

#endif
