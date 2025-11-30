#include "game.h"

#include <iostream>
#include <cmath>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include "../../common/dto/input_state.h"
#include "../map/map_config_loader.h"

#ifndef COLLISION_PATH
#define COLLISION_PATH "."
#endif

Game::Game(float nitro_duracion)
    : nitro_tiempo(nitro_duracion),
      id_indice(0),
      m(),
      players(),
      pending_inputs(),
      map_table(),
      maps_base_path(COLLISION_PATH),
      races(),
      current_race_index(0),
      state(GameState::Lobby),
      is_finished(false),
      marketplace_time_remaining(0.f),
      pending_race_start(false),
      current_map_id(0),
      city(),
      garage(),
      market(150.0f)
{
    map_table.emplace("ViceCity", "/MapaViceCity.yaml");
    map_table.emplace("LibertyCity",    "/MapaLibertyCity.yaml");
    map_table.emplace("SanAndreas",     "/MapaSanAndreas.yaml");
}

Game::~Game() {
    players.clear();
}

bool Game::jugador_existe_auxiliar(size_t id) { return players.find(id) != players.end(); }

bool Game::player_exists(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    return jugador_existe_auxiliar(id);
}

void Game::throw_jugador_no_existe(size_t id) const {
    throw std::invalid_argument("The player with id " + std::to_string(id) + " does not exist.");
}

std::string Game::resolve_map_path(const std::string& map_id) const {
    auto it = map_table.find(map_id);
    if (it == map_table.end()) {
        throw std::invalid_argument("[Game] Map ID '" + map_id + "' not found in map table.");
    }
    return maps_base_path + it->second;
}

size_t Game::add_player(const std::string& name, uint8_t car_id) {
    std::lock_guard<std::mutex> lock(m);

    if (!garage.reserve_car(car_id)) {
        throw std::invalid_argument(
            "Car ID " + std::to_string(car_id) + " is not available"
        );
    }

    while (jugador_existe_auxiliar(id_indice)) {
        ++id_indice;
    }

    CarModel model = garage.get_car_model(car_id);

    players.emplace(id_indice, Player{id_indice, name, model});
    players.at(id_indice).set_car_id(car_id);

    std::cout << "[Game] Added player id=" << id_indice
              << ", name='" << name
              << "', car_id=" << static_cast<int>(car_id)
              << "\n";
    return id_indice;
}

void Game::remove_player(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    players.erase(id);
    // Si no hay carrera activa aún (ej: lobby esperando), evitamos acceder a races
    if (!races.empty() && current_race_index < races.size()) {
        races[current_race_index].remove_player(id);
    }
}

void Game::register_player_move(size_t id, Movement movimiento) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    InputState& in = pending_inputs[id];
    switch (movimiento) {
        case Movement::Up:    in.up = true;    break;
        case Movement::Down:  in.down = true;  break;
        case Movement::Left:  in.left = true;  break;
        case Movement::Right: in.right = true; break;
        default: break;
    }
}

std::vector<PlayerTickInfo> Game::players_tick_info() {
    std::lock_guard<std::mutex> lock(m);
    if (!has_active_race()) {
        return {};
    }
    return races[current_race_index].snapshot_ticks();
}

std::vector<NpcTickInfo> Game::npcs_tick_info() {
    std::lock_guard<std::mutex> lock(m);
    if (!has_active_race()) {
        return {};
    }
    return races[current_race_index].snapshot_npcs();
}

void Game::update(float dt) {
    std::lock_guard<std::mutex> lock(m);
    if (state == GameState::Lobby) {
        pending_inputs.clear();
        return;
    }

    if (state == GameState::Finished) {
        pending_inputs.clear();
        return;
    }

    if (state == GameState::Marketplace) {
        marketplace_time_remaining -= dt;
        if (marketplace_time_remaining <= 0.0f) {
            finish_market_phase();
        }
        return;
    }

    city.step(dt);

    // 2) drenar SIEMPRE la cola de eventos de checkpoints del mundo físico
    auto events = city.get_world().consume_checkpoint_events();

    // Si todavía no hay carrera activa
    if (!has_active_race()) {
        pending_inputs.clear();
        return;
    }

    // 3) aplicar inputs acumulados (una vez por tick)
    for (const auto& kv : pending_inputs) {
        const size_t pid = kv.first;
        const InputState& in = kv.second;
        get_current_race().apply_input(pid, in);
    }
    pending_inputs.clear();

    // 4) procesar eventos de checkpoints
    for (const auto& ev : events) {
        get_current_race().on_car_checkpoint(ev.race_id, ev.car_id, ev.checkpoint_index);
    }

    // 5) avanzar tiempo de la carrera
    get_current_race().advance_time(dt);

    // 6) chequear fin de carrera
    if (get_current_race().is_finished()) {
        on_race_ended();
    }
}

void Game::on_race_ended() {

    RaceResult results = get_current_race().build_race_results();

    auto penalties_upgrades = market.consume_penalties_for_race();

    // 3) Aplicar resultados de la carrera a los jugadores y sumo las penalizaciones
    apply_race_results_to_players(results, penalties_upgrades);

    auto current_results = build_player_result_current(results, penalties_upgrades);

    set_pending_results(std::move(current_results));

    if (current_race_index + 1 >= races.size()) {
        std::cout << "[Game] All races finished.\n";
        state = GameState::Finished;
        // Construir resultados totales y marcarlos como pendientes para broadcast final
        auto total_results = build_total_results();
        set_pending_total_results(std::move(total_results));
        return;
    }

    // LOGICA DE MARKET:
    // Iniciar fase de Marketplace para que los jugadores elijan mejoras
    start_market_phase();
}

void Game::start_market_phase() {
    std::cout << "[Game] Entering Marketplace for " << MARKET_DURATION << " seconds\n";
    marketplace_time_remaining = MARKET_DURATION;
    state = GameState::Marketplace;
    pending_market_init = true;
}

bool Game::consume_pending_market_init(std::vector<ImprovementResult>& market_init_msgs) {
    if (!pending_market_init) return false;

    pending_market_init = false;

    market_init_msgs.clear();
    market_init_msgs.reserve(players.size());

    for (const auto& kv : players) {
        size_t pid = kv.first;

        PlayerMarketInfo m_info = market.get_total_player_info(pid);
        ImprovementResult msg;

        msg.player_id             = (uint32_t)(pid);
        msg.improvement_id        = (uint8_t)(CarImprovement::Init);
        msg.ok                    = true;
        msg.total_penalty_seconds = 0;
        msg.current_balance       = (uint32_t)(std::round(m_info.balance));

        market_init_msgs.push_back(msg);
    }

    return true;
}

float Game::get_improvement_penalty(CarImprovement imp) const {
    return market.get_improvement_time_penalty(imp);
}

void Game::finish_market_phase() {
    std::cout << "[Game] Marketplace ended. Applying upgrades and starting next race.\n";

    // Aplicar mejoras compradas a cada jugador
    for (auto& kv : players) {
        size_t pid = kv.first;
        Player& player = kv.second;
        CarModel updated = market.apply_upgrades_to_model(pid, player.get_car_model());
        player.set_car_model(updated);
    }

    // avanzar al siguiente Race, si existe
    if (current_race_index + 1 < races.size()) {
        try {
            races[current_race_index].clear_cars();
        } catch (const std::exception& e) {
            std::cerr << "[Game] Error clearing previous race cars: " << e.what() << "\n";
        }
        ++current_race_index;
        std::cout << "[Game] Advancing to race index=" << current_race_index << " of " << races.size() << " total\n";

        start_current_race();
        std::cout << "[Game] start_current_race() invoked after marketplace. state=" << (int)state << "\n";
        // Marcar pendiente RaceStart para que el Gameloop haga broadcast
        pending_race_start = true;
        std::cout << "[Game] pending_race_start set=true (map_id=" << (int)current_map_id << ")\n";
    } else {
        std::cout << "[GAME] All races finished, game over.\n";
        state = GameState::Finished;
    }
}

void Game::apply_race_results_to_players(const RaceResult& race_result, const std::unordered_map<size_t, float>& penalties_seconds) {
    for (const auto& entry : race_result.result) {
        auto player_it = players.find(entry.player_id);
        if (player_it == players.end())
            continue;

        Player& player = player_it->second;

        float base_seconds = entry.finish_time_seconds;

        float penalty_seconds = 0.f;
        auto penalty_it = penalties_seconds.find(entry.player_id);
        if (penalty_it != penalties_seconds.end()) {
            penalty_seconds = penalty_it->second;
        }

        player.register_race_result(base_seconds, penalty_seconds);
    }
}

bool Game::buy_upgrade(size_t player_id, CarImprovement improvement) {
    std::lock_guard<std::mutex> lock(m);
    if (state != GameState::Marketplace) {
        return false;
    }
    if (!jugador_existe_auxiliar(player_id)) {
        return false;
    }
    return market.buy_upgrade(player_id, improvement);
}

void Game::set_player_name(size_t id, std::string name) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    std::cout << "[Game] Setting name for player id=" << id << ": '" << name << "'\n";
    players.at(id).set_name(std::move(name));
}

std::string Game::get_player_name(size_t id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    auto it = players.find(id);
    if (it == players.end()) {
        throw_jugador_no_existe(id);
    }
    return it->second.get_name();
}

TimeTickInfo Game::get_player_race_time(size_t id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    auto it = players.find(id);
    if (it == players.end()) {
        return TimeTickInfo{0};
    }
    if (!has_active_race()) {
        return TimeTickInfo{0};
    }
    return TimeTickInfo{ races[current_race_index].get_race_time_seconds() };
}

Race& Game::get_current_race() {
    if (races.empty()) {
        std::cout << "[Game] ERROR: get_current_race() llamado sin carreras activas\n";
        throw std::runtime_error("No active races in Game");
    }

    if (current_race_index >= races.size()) {
        std::cout << "[Game] ERROR: current_race_index fuera de rango: "
                  << current_race_index << " (size=" << races.size() << ")\n";
        throw std::runtime_error("current_race_index out of range");
    }

    return races[current_race_index];
}

bool Game::has_active_race() const {
    return !races.empty()
        && current_race_index < races.size()
        && state == GameState::Racing;
}

bool Game::has_active_market_place() const {
    return state == GameState::Marketplace;
}

std::vector<PlayerResultCurrent> Game::build_player_result_current(const RaceResult& race_result, const std::unordered_map<size_t, float>& penalties_seconds) const {
    std::vector<PlayerResultCurrent> packed;

    for (const auto& entry : race_result.result) {
        auto player_it = players.find(entry.player_id);
        if (player_it == players.end()) {
            continue;
        }

        const Player& player = player_it->second;

        float base_seconds = entry.finish_time_seconds;

        float penalty_seconds = 0.f;
        auto penalty_it = penalties_seconds.find(entry.player_id);
        if (penalty_it != penalties_seconds.end()) {
            penalty_seconds = static_cast<float>(penalty_it->second);
        }

        float race_time = base_seconds + penalty_seconds;
        float total_time = player.get_total_time_seconds();

        PlayerResultCurrent result{};
        result.player_id          = entry.player_id;
        result.username           = player.get_name();
        result.race_time_seconds  = (uint32_t)(race_time);
        result.total_time_seconds = (uint32_t)(total_time);
        result.position           = (uint8_t)(entry.position);

        packed.push_back(result);
    }

    return packed;
}

bool Game::has_pending_results() const{
    return pending_results;
}

bool Game::comsume_pending_results(std::vector<PlayerResultCurrent>& current) {
    if (!pending_results) return false;
    current = std::move(last_results_current);
    pending_results = false;
    return true;
}

void Game::set_pending_results(std::vector<PlayerResultCurrent>&& current) {
    last_results_current = std::move(current);
    pending_results = true;
}

std::vector<PlayerResultTotal> Game::build_total_results() const {
    std::vector<std::pair<std::string, float>> totals;
    totals.reserve(players.size());
    for (const auto& kv : players) {
        const Player& p = kv.second;
        totals.emplace_back(p.get_name(), p.get_total_time_seconds());
    }
    std::sort(totals.begin(), totals.end(), [](const auto& a, const auto& b){ return a.second < b.second; });

    std::vector<PlayerResultTotal> out;
    out.reserve(totals.size());
    uint8_t pos = 1;
    for (const auto& t : totals) {
        PlayerResultTotal r{};
        r.username = t.first;
        r.total_time_seconds = static_cast<uint32_t>(t.second);
        r.position = pos++;
        out.push_back(r);
    }
    return out;
}

void Game::set_pending_total_results(std::vector<PlayerResultTotal>&& total) {
    last_results_total = std::move(total);
    pending_total_results = true;
}

bool Game::has_pending_total_results() const {
    return pending_total_results;
}

bool Game::consume_pending_total_results(std::vector<PlayerResultTotal>& total) {
    if (!pending_total_results) return false;
    total = std::move(last_results_total);
    pending_total_results = false;
    return true;
}

PlayerMarketInfo Game::get_player_market_info(size_t player_id) const {
    return market.get_total_player_info(player_id);
}

void Game::start_current_race() {
    //std::lock_guard<std::mutex> lock(m);
    if (state == GameState::Racing) {
        std::cout << "[Game] start_current_race() early exit: already Racing (index=" << current_race_index << ")\n";
        return;
    }
    
    Race& r = get_current_race();
    const std::string& route = r.get_route_id();

    size_t spawn_index = 0;
    for (auto& kv : players) {
        const size_t player_id = kv.first;
        Player& player = kv.second;
        SpawnPoint sp = city.get_spawn_for_index(spawn_index++, route);
        std::cout << "[DebugPlayer] player " << player_id << " car_model.life=" << player.get_car_model().life << "\n";
        r.add_player(player_id, player.get_car_model(), player.get_car_id(), sp.x_px, sp.y_px);
    }
    // --- Inicializar NPCs proceduralmente ---
    size_t npc_count = 10; // Puedes ajustar la cantidad
    r.init_npc_spawns(city, npc_count);

    state = GameState::Racing;
    std::cout << "[Game] Race " << current_race_index << " started (players=" << players.size() << ") state set=Racing\n";
}

bool Game::consume_pending_race_start(uint8_t& map_id) {
    std::lock_guard<std::mutex> lock(m);
    if (!pending_race_start) {
        return false;
    }
    map_id = current_map_id;
    pending_race_start = false;
    return true;
}

void Game::load_map(const MapConfig& cfg) {
    std::lock_guard<std::mutex> lock(m);
    city.load_map(cfg);
    init_races();
    state = GameState::Lobby;
}

void Game::load_map_by_id(const std::string& map_id) {
    const std::string ruta = resolve_map_path(map_id);
    MapConfig cfg = MapConfigLoader::load_tiled_file(ruta);
    load_map(cfg);
    if (map_id == "LibertyCity") current_map_id = 0;
    else if (map_id == "SanAndreas") current_map_id = 1;
    else if (map_id == "ViceCity") current_map_id = 2;
    else current_map_id = 0;
}

TimeTickInfo Game::get_race_time() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    if (!has_active_race()) {
        return TimeTickInfo{0};
    }
    return TimeTickInfo{ races[current_race_index].get_race_time_seconds() };
}

TimeTickInfo Game::get_market_time() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    if (state != GameState::Marketplace) {
        return TimeTickInfo{0};
    }
    float remaining = marketplace_time_remaining;
    if (remaining < 0.f) remaining = 0.f;
    return TimeTickInfo{ (uint32_t)std::ceil(remaining) };
}

void Game::init_races() {
    PhysicsWorld& world = city.get_world();
    races.clear();
    
    auto route_ids = city.get_route_ids();
    
    uint32_t race_index = 0;
    for (const std::string& route_id : route_ids) {
        Track track = city.build_track(route_id);

        races.emplace_back(race_index, world);
        races.back().set_track(track);

        std::cout << "[Game] Initialized race " << race_index
                  << " with route " << route_id
                  << " (checkpoints=" << track.checkpoint_count << ")\n";

        ++race_index;
    }
}

void Game::apply_cheat(size_t player_id, uint8_t cheat_code) {
    std::lock_guard<std::mutex> lk(m);
    if (!has_active_race()) return;
    get_current_race().apply_cheat(player_id, cheat_code);
}

void Game::set_player_infinite_life(size_t player_id, bool enable) {
    std::lock_guard<std::mutex> lock(m);
    auto it = players.find(player_id);
    if (it == players.end()) return;
    it->second.set_infinite_life(enable);
}
