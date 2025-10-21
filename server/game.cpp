#include "game.h"

#include <iostream>
#include <utility>

Game::Game(float nitro_duracion): nitro_tiempo(nitro_duracion) {}

Game::~Game() {
    nitros_activo.clear();
    jugadores.clear();
}

bool Game::jugador_existe_auxiliar(size_t id) { return jugadores.find(id) != jugadores.end(); }

bool Game::jugador_existe(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    return jugador_existe_auxiliar(id);
}

void Game::throw_jugador_no_existe(size_t id) const {
    throw std::invalid_argument("The player with id " + std::to_string(id) + " does not exist.");
}

size_t Game::jugador_agregar() {
    std::lock_guard<std::mutex> lock(m);
    while (jugador_existe_auxiliar(id_indice)) {
        id_indice++;
    }

    jugador_info info_jugador;
    info_jugador.id = id_indice;
    jugadores[id_indice] = std::move(info_jugador);

    return id_indice;
}

void Game::jugador_eliminar(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    if (jugadores[id].nitro_activo) {
        for (auto it = nitros_activo.begin(); it != nitros_activo.end(); ++it) {
            if (it->id == id) {
                nitros_activo.erase(it);
                break;
            }
        }
    }
    jugadores.erase(id);
}

bool Game::jugador_activar_nitro(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    if (jugadores[id].nitro_activo) {
        return false;
    }

    data_jugador nuevo_nitro;
    nuevo_nitro.id = id;
    nuevo_nitro.nitro_tiempo_restante = nitro_tiempo;
    nitros_activo.push_back(std::move(nuevo_nitro));
    jugadores[id].nitro_activo = true;

    return true;
}

bool Game::jugador_tiene_nitro_activo(size_t id) {
    std::lock_guard<std::mutex> lock(m);
    if (!jugador_existe_auxiliar(id)) {
        throw_jugador_no_existe(id);
    }
    return jugadores[id].nitro_activo;
}

std::vector<size_t> Game::tiempo_trascurrido(float tiempo) {
    std::lock_guard<std::mutex> lock(m);
    std::vector<size_t> desactivados;
    for (auto it = nitros_activo.begin(); it != nitros_activo.end();) {
        it->nitro_tiempo_restante -= tiempo;
        if (it->nitro_tiempo_restante <= 0) {
            jugadores[it->id].nitro_activo = false;
            desactivados.push_back(it->id);
            it = nitros_activo.erase(it);
        } else {
            ++it;
        }
    }
    return desactivados;
}

int Game::cantidad_nitros_activos() {
    std::lock_guard<std::mutex> lock(m);
    return nitros_activo.size();
}
