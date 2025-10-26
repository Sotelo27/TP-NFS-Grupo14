#include "player.h"
#include <utility>

Player::Player(size_t id, std::string name, Car car)
    : id(id), name(std::move(name)), car(std::move(car)) {}

size_t Player::get_Id() const {
    return id;
}

const std::string& Player::get_name() const {
    return name;
}

void Player::execute_movement(Movement move) {
    car.move(move);
}

Position Player::get_Pose() const {
    return car.get_Pose();
}
