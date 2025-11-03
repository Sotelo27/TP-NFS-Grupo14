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

size_t Game::add_player() { // Luego necesito que reciba un player o el  nombre y auto
    std::lock_guard<std::mutex> lock(m);
    while (jugador_existe_auxiliar(id_indice)) {
        id_indice++;
    }

    players.emplace(id_indice, Player{id_indice});
    std::cout << "[Game] Added player with id=" << id_indice << "\n";

    // Agregar jugador a la race minima con su CarModel y una posición inicial segura lejos de bordes
    // El CarModel se obtiene del player recién creado
    const CarModel& model = players.at(id_indice).get_car_model();
    race.add_player(id_indice, model, 1000.0f, 1000.0f);

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
    // Acumular inputs para este tick (OR de todos los mensajes recibidos)
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

    const uint32_t dtMs = (uint32_t)std::lround(dt * 1000.0f);
    race.update(dtMs);
}

void Game::set_player_name(size_t id, std::string name) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    std::cout << "[Game] Setting name for player id=" << id << ": '" << name << "'\n";
    players.at(id).set_name(std::move(name));
}