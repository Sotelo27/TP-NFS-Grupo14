#ifndef MARKET_H
#define MARKET_H

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

#include "market_info.h"
#include "../../common/car_model.h"
#include "../../common/enum/car_improvement.h"


class Market {
private:
    std::unordered_map<CarImprovement, UpgradeInfo> catalog_upgrades;
    std::unordered_map< size_t, PlayerMarketInfo> player_market_info;
    float base_balance;

public:
    Market(float base_balance_init);

    /*
     * Resetea todas las mejoras compradas por los jugadores
     */
    void reset_upgrades();

    /*
     * Compra una mejora para un jugador
     */
    bool buy_upgrade(size_t player_id, CarImprovement upgrade_id);
    
    /*
     * Busca la informacion de una mejora en el catalogo
     */
    const UpgradeInfo& find_info_upgrade(CarImprovement id) const;

    /*
     * Aplica las mejoras compradas por un jugador a su modelo de auto base
     */
    CarModel apply_upgrades_to_model(std::size_t player_id, const CarModel& base) const;

    /*
     * Consume y devuelve las penalizaciones de tiempo acumuladas
     * por cada jugador para la carrera actual
     */
    std::unordered_map<std::size_t, float> consume_penalties_for_race();

    /*
     * Obtiene la informacion total del jugador en el mercado
     */
    PlayerMarketInfo get_total_player_info(size_t player_id) const;

    /*
     * Obtiene la penalizacion de tiempo asociada a una mejora
     */
    float get_improvement_time_penalty(CarImprovement id) const;
    
};

#endif