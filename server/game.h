#ifndef GAME_H
#define GAME_H

#include <list>
#include <map>
#include <mutex>
#include <string>
#include <vector>

struct data_jugador {
    size_t id;
    float nitro_tiempo_restante;
};

struct jugador_info {
    size_t id;
    bool nitro_activo = false;
};

class Game {
private:
    float nitro_tiempo;
    std::map<int, jugador_info> jugadores;
    std::list<data_jugador> nitros_activo;
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
    bool jugador_existe(size_t id);

    /*
     * Agrega un jugador al game.
     *
     * Retorna el ID del jugador agregado.
     */
    size_t jugador_agregar();

    /*
     * Elimina un jugador del game.
     *
     * Lanza invalid_argument si el jugador no existe.
     */
    void jugador_eliminar(size_t id);

    /*
     * Activa el nitro de un jugador.
     * Lanza invalid_argument si el jugador no existe.
     *
     * No hace nada si el jugador ya tiene el nitro activo y retorna false.
     * Retorna true si el nitro se activo correctamente.
     */
    bool jugador_activar_nitro(size_t id);

    /*
     * Verifica si un jugador tiene el nitro activo.
     * Lanza invalid_argument si el jugador no existe.
     *
     * Retorna true si el jugador tiene el nitro activo, false en caso contrario.
     */
    bool jugador_tiene_nitro_activo(size_t id);

    /*
     * Actualiza el estado del game con el tiempo transcurrido.
     * Disminuye el tiempo de nitro activo de los jugadores.
     *
     * Si el tiempo de nitro de un jugador llega a 0, se desactiva su nitro.
     * Retorna un vector con los IDs de los jugadores cuyo nitro se desactivo.
     */
    std::vector<size_t> tiempo_trascurrido(float tiempo);

    /*
     * Retorna la cantidad de jugadores que tienen el nitro activo.
     */
    int cantidad_nitros_activos();


    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    Game(Game&&) = default;
    Game& operator=(Game&&) = default;

    ~Game();
};

#endif  // GAME_H
