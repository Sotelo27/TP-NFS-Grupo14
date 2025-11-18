#include "game.h"

#include <iostream>
#include <utility>
#include "../../common/dto/input_state.h"
#include "../map/map_config_loader.h"

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
      city(),
      garage()
{
    map_table.emplace("CollisionTest2", "/CollisionTest2.yaml");
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

void Game::apply_player_move(size_t id, Movement movimiento) {
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

void Game::update(float dt) {
    std::lock_guard<std::mutex> lock(m);
    // 1) avanzar simulacion del mundo físico SIEMPRE
    city.step(dt);

    // 2) drenar SIEMPRE la cola de eventos de checkpoints del mundo físico (mantener esta línea)
    auto events = city.get_world().consume_checkpoint_events();

    // Si todavía no hay carrera activa, descartamos inputs y eventos y salimos.
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
    // 1) Leer resultados de la race actual (si querés)
    //    Por ahora podés omitirlo o armar despues.
    //    Ej: auto results = current_race().results();

    // 2) Destruir los autos actuales del PhysicsWorld
    //get_current_race().clear_cars();  // método que llame a physics.destroy_body para cada player

    // 3) Avanzar al siguiente Race, si existe
    if (current_race_index + 1 < races.size()) {
        ++current_race_index;
        state = GameState::Racing; // siguiente carrera

        // 4) Respawnear jugadores para la nueva Race
        //setup_players_for_race(get_current_race());  // arma los autos en nuevos spawns, race_duration=0, etc.
    } else {
        //game_finished = true;
        std::cout << "[GAME] All races finished, game over.\n";
        state = GameState::Finished;
    }
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

uint8_t Game::get_player_health(size_t id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    auto it = players.find(id);
    if (it == players.end()) {
        return 100; // Valor por defecto si no existe
    }
    // TODO: Implementar cuando Player tenga atributo de vida
    return 100; // Por ahora retornar vida completa
}

TimeTickInfo Game::get_player_race_time(size_t id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    auto it = players.find(id);
    if (it == players.end()) {
        return TimeTickInfo{0}; // Valor por defecto si no existe
    }
    // Por ahora devolvemos el tiempo global de la carrera (no individual)
    if (!has_active_race()) {
        return TimeTickInfo{0};
    }
    return TimeTickInfo{ races[current_race_index].get_race_time_seconds() };
}

Race& Game::get_current_race() {
    if (races.empty()) {
        std::cout << "[Game] ERROR: get_current_race() llamado sin carreras activas\n";
        // podés:
        //  - lanzar una excepción más clara:
        throw std::runtime_error("No active races in Game");
        //  - o devolver una referencia a algún dummy, pero no lo recomiendo.
    }

    if (current_race_index >= races.size()) {
        std::cout << "[Game] ERROR: current_race_index fuera de rango: "
                  << current_race_index << " (size=" << races.size() << ")\n";
        throw std::runtime_error("current_race_index out of range");
    }

    return races[current_race_index]; // o .at(current_race_index) si querés checks extra
}

bool Game::has_active_race() const {
    return !races.empty()
        && current_race_index < races.size()
        && state == GameState::Racing;
}


void Game::start_current_race() {
    std::lock_guard<std::mutex> lock(m);
    if (state == GameState::Racing) {
        return;
    }
    
    Race& r = get_current_race();
    const std::string& route = r.get_route_id();

    size_t spawn_index = 0;
    for (auto& kv : players) {
        const size_t player_id = kv.first;
        Player& player = kv.second;
        SpawnPoint sp = city.get_spawn_for_index(spawn_index++, route);
        r.add_player(player_id, player.get_car_model(), player.get_car_id(), sp.x_px, sp.y_px);
        std::cout << "[Game] Spawned player_id=" << player_id
                  << " at (" << sp.x_px << ", " << sp.y_px << ")"
                  << " route='" << route << "'"
                  << " spawn_idx=" << (spawn_index - 1)
                  << " car_id=" << (int)player.get_car_id() << "\n";
    }

    std::cout << "[Game] Race " << current_race_index << " started with "
              << players.size() << " players\n";
    state = GameState::Racing;
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

    std::cout << "[Game] Loaded map '" << map_id << "' from " << ruta
              << " (rects=" << cfg.rects.size()
              << ", polys=" << cfg.polylines.size()
              << ", spawns=" << cfg.spawns.size() << ")\n";
}

TimeTickInfo Game::get_race_time() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    if (!has_active_race()) {
        return TimeTickInfo{0};
    }
    return TimeTickInfo{ races[current_race_index].get_race_time_seconds() };
}

void Game::init_races() {
    PhysicsWorld& world = city.get_world();
    races.emplace_back(0, world);
    races.back().set_track(city.build_track("A"));
}


