#include "player.h"
#include <utility>

Player::Player(size_t id, std::string name, Car car)
    : id(id), name(std::move(name)), car(std::move(car)) {}

Player::Player(size_t id)
    : id(id),
      name("Player N° " + std::to_string(id)),
      car("Model " + std::to_string(id), 200, 0, 0, 0, 100) {
    car.set_Pose(0.f, 0.f); // posicion inicial
}

void Player::execute_movement(Movement move) {
    car.move(move);
}

void Player::update(float dt) noexcept {
    car.update(dt);
}

size_t Player::get_Id() const {
    return id;
}

const std::string& Player::get_name() const {
    return name;
}

Position Player::get_Pose() const {
    return car.get_Pose();
}

void Player::set_name(std::string name) {
    this->name = std::move(name);
}
