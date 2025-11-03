#include "player.h"
#include <utility>

Player::Player(size_t id, std::string name, CarModel car)
    : id(id), name(std::move(name)), car(std::move(car)) {}

Player::Player(size_t id)
    : id(id),
      name("Player N° " + std::to_string(id)) {
    // LO inicialio por ahora asi porque no tengo la config de todo para crear un CarModel segun el auto elegido.
    car.modelo = "Model " + std::to_string(id);
}

size_t Player::get_Id() const {
    return id;
}

const std::string& Player::get_name() const {
    return name;
}

void Player::set_name(std::string name) {
    this->name = std::move(name);
}
