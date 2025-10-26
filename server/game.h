#ifndef GAME_H
#define GAME_H

#include <list>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "../common/base_protocol.h"
#include "../common/player_aux.h"

#include "player.h"

class Game {
private:
    float nitro_tiempo;
    std::map<size_t, Player> players;
    size_t id_indice = 0;
    std::mutex m;


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
     * Obtiene la posición actual del jugador
    */
    std::pair<int16_t, int16_t> get_player_position(size_t id);

    /*
     * Devuelve Player {(id, position)} de todos los jugadores
     */
    std::vector<PlayerPos> players_positions();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    ~Game();
};

#endif  // GAME_H
