#include "game.h"

#include <iostream>
#include <utility>

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

    return id_indice;
}

void Game::remove_player(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    players.erase(id);
}

void Game::apply_player_move(size_t id, Movement movimiento) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }

    auto& j = players.at(id);
    j.execute_movement(movimiento);
}

std::pair<int16_t, int16_t> Game::get_player_position(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    const auto& j = players.at(id);
    Position p = j.get_Pose();
    return {(int16_t)(p.x), (int16_t)(p.y)};
}

std::vector<PlayerPos> Game::players_positions() {
    std::lock_guard<std::mutex> lock(m);
    std::vector<PlayerPos> player_positions;
    player_positions.reserve(players.size());
    for (const auto& [id, player] : players) {
        auto pos = player.get_Pose();
        player_positions.push_back(PlayerPos{(uint32_t)id, (int16_t)(pos.x), (int16_t)(pos.y)});
    }
    return player_positions;
}

void Game::update(float dt) {
    std::lock_guard<std::mutex> lock(m);
    for (auto& [id, j] : players) {
        j.update(dt);
    }
}