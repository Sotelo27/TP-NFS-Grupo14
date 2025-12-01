#ifndef GAME_H
#define GAME_H

#include <list>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../../common/dto/movement.h"
#include "../../common/player_aux.h"
#include "../../common/dto/map_tick_info.h"
#include "../../common/dto/map_config.h"

#include "../Player/player.h"
#include "race.h"
#include "city.h"
#include "garage.h" 
#include "market.h"
#include "../../common/enum/car_improvement.h"
#include "../../common/dto/results_info.h"

#define MARKET_DURATION 30.0f

enum class GameState {
    Lobby,
    Racing,
    Marketplace,
    Finished
};

class Game {
private:
    float nitro_tiempo;
    size_t id_indice;
    std::mutex m;
    
    std::map<size_t, Player> players;
    std::map<size_t, InputState> pending_inputs;
    std::map<std::string, std::string> map_table;// nombre mapa, ruta archivo
    std::string maps_base_path;
    std::vector<Race> races;
    size_t current_race_index;
    GameState state;
    bool is_finished;
    float marketplace_time_remaining;
    bool pending_race_start{false};
    uint8_t current_map_id{0};
    
    City city;
    Garage garage;
    Market market;

    std::vector<PlayerResultCurrent> last_results_current;
    bool pending_results{false};

    // Resultados totales (acumulados al finalizar el juego)
    std::vector<PlayerResultTotal> last_results_total;
    bool pending_total_results{false};
    bool pending_market_init{false};


    void throw_jugador_no_existe(size_t id) const;
    bool jugador_existe_auxiliar(size_t id);

    /*
     * Resuelve la ruta del mapa segun su ID
     */
    std::string resolve_map_path(const std::string& map_id) const;

    /*
     * Inicializa las carreras del juego, por ahora solo 1
     */
    void init_races();

    /*
     * Inicia la fase de marketplace
    */
    void start_market_phase();

    /*
     * Finaliza la fase de marketplace y arranca la siguiente carrera o
     * marca el juego como finalizado si no quedan mas carreras
     */
    void finish_market_phase();

public:
    /*
     * Crea un game con la duracion del nitro especificada en segundos.
     */
    explicit Game(float nitro_duracion);

    /*
     * Verifica si un jugador existe en el game.
     *
     * Retorna true si existe, false en caso contrario.
     */
    bool player_exists(size_t id);

    /*
     * Agrega un jugador al game con nombre y carro especificados.
     *
     * Retorna el ID del jugador agregado.
     */
    size_t add_player(const std::string& name, uint8_t car_id);

    /*
     * Elimina un jugador del game.
     *
     * Lanza invalid_argument si el jugador no existe.
     */
    void remove_player(size_t id);

    /*
     * Registra un movimiento de un jugador (UP, DOWN, LEFT, RIGHT) en map de inputs pendientes
     */
    void register_player_move(size_t id, Movement movimiento);

    /*
     * Actualiza el estado del juego, avanzando el tiempo en dt segundos
     * Por el momento solo actualiza los jugadores
     */
    void update(float dt);

    /*
     * Obtiene el tiempo de carrera actual en segundos
     */
    TimeTickInfo get_race_time() const;

    /*
     * Obtiene el tiempo restante de la fase de Marketplace en segundos
     */
    TimeTickInfo get_market_time() const;

    /*
     * Devuelve informacion del jugador para el tick del mapa (posiciones + salud por ahora)
     */
    std::vector<PlayerTickInfo> players_tick_info();

    /*
     * Devuelve el snapshot de los NPCs para el tick del mapa
     */
    std::vector<NpcTickInfo> npcs_tick_info();
    
    /*
     * Devuelve los eventos de la carrera para el tick del mapa
     */
    std::vector<EventInfo> consume_race_events();

    /*
     * Setea el nombre del jugador
     */
    void set_player_name(size_t id, std::string name);

    /*
     * Obtiene el nombre del jugador
     */
    std::string get_player_name(size_t id) const;

    /*
     * Obtiene el tiempo de carrera del jugador en milisegundos
     * Retorna 0 si no está en carrera
     */
    TimeTickInfo get_player_race_time(size_t id) const;

    /*
     * Obtiene la carrera actual
     */
    Race& get_current_race();

    /*
     * Verifica si hay una carrera activa en curso
     */
    bool has_active_race() const;

    /*
     * Verifica si la fase de marketplace esta en curso
     */
    bool has_active_market_place() const;

    /*
     * Carga el MapConfig paredes, edificios en la ciudad.
     */
    void load_map(const MapConfig& cfg);

    /*
     * Maneja la logica cuando la carrera termina
     */
    void on_race_ended();

    /*
     * Aplica los resultados de la carrera y de las penalizaciones a los jugadores
     */
    void apply_race_results_to_players(const RaceResult& race_result, const std::unordered_map<size_t, float>& penalties_seconds);

    /*
     * Compra una mejora para un jugador durante Marketplace
     */
    bool buy_upgrade(size_t player_id, CarImprovement improvement);


    /*
     * Setea los resultados de la carrera actual para ser consumidos
     */
    void set_pending_results(std::vector<PlayerResultCurrent>&& current);

    /*
     * INdica  si hay resultados pendientes para ser consumidos
     */
    bool has_pending_results() const;

    /*
     * Obtiene los resultados de la carrera actual
     */
    bool comsume_pending_results(std::vector<PlayerResultCurrent>& current);

    /*
     * Construye los resultados totales (acumulados) ordenados por tiempo total
     */
    std::vector<PlayerResultTotal> build_total_results() const;

    /*
     * Setea los resultados totales de la partida para ser consumidos
     */
    void set_pending_total_results(std::vector<PlayerResultTotal>&& total);

    /*
     * Indica si hay resultados totales pendientes para ser consumidos
     */
    bool has_pending_total_results() const;

    /*
     * Consume los resultados totales pendientes
     */
    bool consume_pending_total_results(std::vector<PlayerResultTotal>& total);

    /*
     * Consume el evento de inicio de marketplace
     */
    bool consume_pending_market_init(std::vector<ImprovementResult>& out);
    
    /*
     * Obtiene la penalizacion de tiempo asociada a una mejora
     */
    float get_improvement_penalty(CarImprovement imp) const;

    /*
     * Construye el resultado de la carrera actual para su correcto envio
     */
    std::vector<PlayerResultCurrent> build_player_result_current(const RaceResult& race_result,const std::unordered_map<size_t, float>& penalties_seconds) const;
    
    /*
     * Obtiene la informacioon del jugador de market
     */
    PlayerMarketInfo get_player_market_info(size_t player_id) const;
    
    /*
     * Carga el mapa por su ID segun lo que me mande el cliente
     */
    void load_map_by_id(const std::string& map_id);

    /*
     * Inicia la carrera actual agregando todos los players al Race
     * y posicionandolos segun los spawns definidos en city
     */
    void start_current_race();

    /*
     * Consume el evento de carrera iniciada
     */
    bool consume_pending_race_start(uint8_t& map_id);

    /*
     * Aplica un cheat recibido por el cliente
     */
    void apply_cheat(size_t player_id, uint8_t cheat_code);

    // NUEVO: setear vida infinita a un jugador
    void set_player_infinite_life(size_t player_id, bool enable);

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    ~Game();
};

#endif  // GAME_H
