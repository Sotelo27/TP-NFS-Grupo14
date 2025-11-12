#ifndef GAME_H
#define GAME_H

#include <list>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../common/dto/movement.h"
#include "../common/player_aux.h"

#include "Player/player.h"
#include "race.h"
#include "city.h"
#include "../common/dto/map_config.h"
#include "../common/dto/map_tick_info.h"

class Game {
private:
    float nitro_tiempo;
    std::map<size_t, Player> players;
    size_t id_indice = 0;
    std::mutex m;
    City city; // ciudad única (mundo físico compartido)
    Race race{1, city.get_world()}; // Carrera dentro de la ciudad
    // Inputs acumulados por tick (OR de todos los mensajes recibidos en el frame)
    std::map<size_t, InputState> pending_inputs;


    void throw_jugador_no_existe(size_t id) const;
    bool jugador_existe_auxiliar(size_t id);

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
     * Agrega un jugador al game.
     *
     * Retorna el ID del jugador agregado.
     */
    size_t add_player();

    /*
     * Elimina un jugador del game.
     *
     * Lanza invalid_argument si el jugador no existe.
     */
    void remove_player(size_t id);

    /*
     * Aplica un movimiento discreto al jugador
     */
    void apply_player_move(size_t id, Movement movimiento);

    /*
     * Actualiza el estado del juego, avanzando el tiempo en dt segundos
     * Por el momento solo actualiza los jugadores
     */
    void update(float dt);

    /*
     * Devuelve Player {(id, position)} de todos los jugadores
     */
    std::vector<PlayerPos> players_positions();

    /*
     * Devuelve informacion del jugador para el tick del mapa (posiciones + salud por ahora)
     */
    std::vector<PlayerTickInfo> players_tick_info();

    /*
     * Setea el nombre del jugador
     */
    void set_player_name(size_t id, std::string name);

    /*
     * Obtiene el nombre del jugador
     */
    std::string get_player_name(size_t id) const;

    /*
     * Obtiene la vida del jugador (0-100)
     * Retorna 100 por defecto si no está implementado
     */
    uint8_t get_player_health(size_t id) const;

    /*
     * Obtiene el tiempo de carrera del jugador en milisegundos
     * Retorna 0 si no está en carrera
     */
    uint32_t get_player_race_time(size_t id) const;

    /*
     * Carga el MapConfig paredes, edificios en la ciudad.
     */
    void load_map(const MapConfig& cfg);

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    ~Game();
};

#endif  // GAME_H
