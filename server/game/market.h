#ifndef MARKET_H
#define MARKET_H

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>

#include "market_info.h"
#include "../../common/car_model.h"

class Market {
private:
    std::unordered_map<UpgradeId, UpgradeInfo> catalog_upgrades;
    std::unordered_map< size_t, PlayerMarketInfo> player_market_info;

public:
    Market();

    /*
     * Resetea todas las mejoras compradas por los jugadores
     */
    void reset_upgrades();

    /*
     * Compra una mejora para un jugador
     */
    bool buy_upgrade(size_t player_id, UpgradeId upgrade_id);
    
    /*
     * Busca la informacion de una mejora en el catalogo
     */
    const UpgradeInfo& find_info_upgrade(UpgradeId id) const;

    /*
     * Aplica las mejoras compradas por un jugador a su modelo de auto base
     */
    CarModel apply_upgrades_to_model(std::size_t player_id, const CarModel& base) const;

    /*
     * Consume y devuelve las penalizaciones de tiempo acumuladas
     * por cada jugador para la carrera actual
     */
    std::unordered_map<std::size_t, std::uint32_t> consume_penalties_for_race();

    PlayerMarketInfo get_total_player_info(size_t player_id) const;

    std::vector<UpgradeId> get_upgrades(size_t player_id) const;
    
};

#endif