#include "game.h"

#include <iostream>
#include <utility>
#include "../common/dto/input_state.h"

Game::Game(float nitro_duracion): nitro_tiempo(nitro_duracion) {}

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
    
    //std::cout << "[Game] Spawning player " << id_indice << " at position (" 
    //          << sp.x_px << ", " << sp.y_px << ")\n";
    //
    //// IMPORTANTE: Si no hay spawns configurados, usar una posición visible por defecto
    //if (sp.x_px == 0 && sp.y_px == 0) {
    //    std::cout << "[Game] WARNING: No spawn configured, using default position (400, 300)\n";
    //    sp.x_px = 400;
    //    sp.y_px = 300;
    //}
    
    race.add_player(id_indice, model, sp.x_px, sp.y_px);

    return id_indice;
}

void Game::remove_player(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
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

void Game::update(float dt) {
    std::lock_guard<std::mutex> lock(m);
    // Aplicar inputs acumulados de todos los jugadores una sola vez por tick
    for (const auto& kv : pending_inputs) {
        const size_t pid = kv.first;
        const InputState& in = kv.second;
        race.apply_input(pid, in);
    }
    pending_inputs.clear();

    // El step del mundo físico se hace a nivel de ciudad
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

void Game::load_map(const MapConfig& cfg) {
    std::lock_guard<std::mutex> lock(m);
    city.load_map(cfg);
}