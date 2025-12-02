#ifndef RESULT_H
#define RESULT_H

#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>
#include "../../common/dto/results_info.h"
#include "race_info.h"
#include "../Player/player.h"

class Result {
private:
    std::vector<PlayerResultCurrent> last_results_current;
    bool pending_results{false};

    std::vector<PlayerResultTotal> last_results_total;
    bool pending_total_results{false};

public:
    Result() = default;
    ~Result() = default;

    /*
     * Guarda los resultados de la carrera actual
     */
    void set_pending_results(std::vector<PlayerResultCurrent>&& current);
    
    /*
     * Indica si hay resultados de la carrera actual pendientes para que game los consuma
     */
    bool has_pending_results() const;
    
    /*
     * Consume los resultados pendientes de la carrera actual y los devuelve en out
     */
    bool consume_pending_results(std::vector<PlayerResultCurrent>& out);

    /*
     * Guarda los resultados totales acumulados de la partida
     */
    void set_pending_total_results(std::vector<PlayerResultTotal>&& total);
    
    /*
     * Indica si hay resultados totales pendientes para que game los consuma
     */
    bool has_pending_total_results() const;

    /*
     * Consume los resultados totales pendientes al finalizar la partida y los devuelve en out
     */
    bool consume_pending_total_results(std::vector<PlayerResultTotal>& out);

    /*
     * Construye los resultados de la carrera actual con la informacion obtenida de RaceResult
     */
    std::vector<PlayerResultCurrent> build_player_result_current(const std::map<size_t, Player>& players, const RaceResult& race_result, const std::unordered_map<size_t, float>& penalties_seconds) const;

    /*
     * Construye los resultados totales de la partida
     */
    std::vector<PlayerResultTotal> build_total_results(const std::map<size_t, Player>& players) const;

    Result(const Result&) = delete;
    Result& operator=(const Result&) = delete;
    Result(Result&&) = default;
    Result& operator=(Result&&) = default;
};

#endif
