#include "game.h"

#include <iostream>
#include <utility>
#include "../../common/dto/input_state.h"
#include "../map/map_config_loader.h"

#ifndef COLLISION_PATH
#define COLLISION_PATH "."
#endif

Game::Game(float nitro_duracion)
        : nitro_tiempo(nitro_duracion),
            id_indice(0),
            players(),
            pending_inputs(),
            map_table(),
            maps_base_path(COLLISION_PATH),
            m(),
            city(),
            race(1, city.get_world()),
            garage()
{
    map_table.emplace("CollisionTest2", "/CollisionTest2.yaml");
    map_table.emplace("LibertyCity",    "/MapaLibertyCity.yaml");
    map_table.emplace("SanAndreas",     "/MapaSanAndreas.yaml"); // <-- debe ser exactamente así
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

//TODO por ahora lo dejo pero lo tengo que eliminar
size_t Game::add_player() {
    std::lock_guard<std::mutex> lock(m);
    while (jugador_existe_auxiliar(id_indice)) {
        id_indice++;
    }

    players.emplace(id_indice, Player{id_indice});
    std::cout << "[Game] Added player with id=" << id_indice << "\n";

    // Agregar jugador a la race con su CarModel y el spawn de la ciudad
    const CarModel& model = players.at(id_indice).get_car_model();
    size_t spawn_index = (players.size() > 0) ? (players.size() - 1) : 0;
    SpawnPoint sp = city.get_spawn_for_index(spawn_index);
    
    race.add_player(id_indice, model, sp.x_px, sp.y_px);
    return id_indice;
}

//TODO: eL MONITOR DEBE USAR ESTA FUNCION
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


    size_t spawn_index = (players.size() > 0) ? (players.size() - 1) : 0;
    SpawnPoint sp = city.get_spawn_for_index(spawn_index);

    race.add_player(id_indice, model, sp.x_px, sp.y_px);

    std::cout << "[Game] Player " << name
              << " (id=" << id_indice
              << ") spawned at (" << sp.x_px << ", " << sp.y_px << ")\n";

    return id_indice;
}

void Game::remove_player(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    Player& p = players.at(id);
    uint8_t car_id = p.get_car_id();
    // libero el auto en el garage por si se desconecta antes de en el lobby o en la carrera
    garage.release_car(car_id);
    players.erase(id);
    race.remove_player(id);
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

std::vector<PlayerPos> Game::players_positions() {
    std::lock_guard<std::mutex> lock(m);
    return race.snapshot_poses();
}

std::vector<PlayerTickInfo> Game::players_tick_info() {
    std::lock_guard<std::mutex> lock(m);
    return race.snapshot_ticks();
}

void Game::update(float dt) {
    std::lock_guard<std::mutex> lock(m);
    // Aplicar inputs acumulados de todos los jugadores una sola vez por tick
    for (const auto& kv : pending_inputs) {
        const size_t pid = kv.first;
        const InputState& in = kv.second;
        race.apply_input(pid, in);
    }
    pending_inputs.clear();

    // avanza simulacion del mundo fisico
    city.step(dt);
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

uint32_t Game::get_player_race_time(size_t id) const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m));
    auto it = players.find(id);
    if (it == players.end()) {
        return 0; // Valor por defecto si no existe
    }
    // TODO: Implementar cuando se tenga sistema de tiempo de carrera
    return 0; // Por ahora retornar 0
}

void Game::load_map(const MapConfig& cfg) {
    std::lock_guard<std::mutex> lock(m);
    city.load_map(cfg);
}

//TODO, el monitor debe usar esta funcion SEGUN EL ID QUE HAYA SELCCIONADO EL CLIENTE
void Game::load_map_by_id(const std::string& map_id) {
    const std::string ruta = resolve_map_path(map_id);

    MapConfig cfg = MapConfigLoader::load_tiled_file(ruta);
    load_map(cfg);

    std::cout << "[Game] Loaded map '" << map_id << "' from " << ruta
              << " (rects=" << cfg.rects.size()
              << ", polys=" << cfg.polylines.size()
              << ", spawns=" << cfg.spawns.size() << ")\n";
    
    //city.set_spawns(cfg.spawns);
}

