#include "player.h"
#include <utility>

Player::Player(size_t id, std::string name, CarModel car)
    : id(id), name(std::move(name)), car(std::move(car)) {}

Player::Player(size_t id)  // TODO: tengo eliminar este constructor, porque no tiene sentido
    : id(id),
      name("Player N° " + std::to_string(id)) {
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
