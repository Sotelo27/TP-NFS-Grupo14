#ifndef MARKET_PHASE_H
#define MARKET_PHASE_H

#include <vector>
#include <map>
#include <unordered_map>
#include <cstdint>

#include "market.h"
#include "../Player/player.h"
#include "../../common/dto/results_info.h"
#include "../../common/enum/car_improvement.h"

#define MARKET_BASE_BALANCE 150.f

class MarketPhase {
private:
    Market market;
    float time_remaining{0.f};
    bool pending_init{false};

public:
    explicit MarketPhase(float /*duration_seconds*/)
        : market(MARKET_BASE_BALANCE), time_remaining(0.f), pending_init(false) {}

    /*
     * Iinicia la fase: resetea estado visual del jugador y marca init pendiente
     */
    void begin(std::map<size_t, Player>& players, float duration_seconds);

    /*
     * Decrementa el tiempo restante de la fase de Market
     */
    void update(float dt);

    /*
     * Tiempo restante en segundos
     */
    float get_time_remaining() const;

    /*
     * COnstruye el mensaje de init del market(uno por jugador)
     */
    bool consume_pending_market_init(const std::map<size_t, Player>& players, std::vector<ImprovementResult>& out);

    /*
     * COmprar una mejora para un jugador
     */
    bool buy_upgrade(size_t player_id, CarImprovement improvement);
    
    /*
     * Obtiene la penalizacion de tiempo asociada a una mejora
     */
    float get_improvement_penalty(CarImprovement imp) const;

    /*
     * Obtiene la informacion total del jugDOR de lo que lleva comprado
     */
    PlayerMarketInfo get_player_market_info(size_t player_id) const;

    /*
     * Aplica las mejoras compradas a los modelos de car de los jugadores
     */
    void apply_upgrades_to_all(std::map<size_t, Player>& players) const;

    /*
     *  Devuelve las penalizaciones acumuladas para la carrera que termina
     */
    std::unordered_map<size_t, float> consume_penalties_for_race();
};

#endif // MARKET_PHASE_H
