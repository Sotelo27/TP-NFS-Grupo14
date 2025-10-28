#include "car.h"
#include <cmath>

#define EPSILON 1e-6f

Car::Car(std::string modelo,
         uint16_t base_speed,
         uint16_t base_acceleration,
         uint16_t base_mass,
         uint16_t base_control,
         uint16_t base_health)
    : model(std::move(modelo)),
      base_speed(base_speed),
      base_acceleration(base_acceleration),
      base_mass(base_mass),
      base_control(base_control),
      base_health(base_health),
      current_health(base_health),
      pos_X(0.f),
      pos_Y(0.f),
      direction_X(0),
      direction_Y(0) {}


void Car::move(Movement mv) noexcept {
    switch (mv) {
        case Movement::Left:  direction_X = -10; direction_Y = 0;  break;
        case Movement::Right: direction_X = 10;  direction_Y = 0;  break;
        case Movement::Up:    direction_X = 0;  direction_Y = -10; break;
        case Movement::Down:  direction_X = 0;  direction_Y = 10;  break;
        default:              direction_X = 0;  direction_Y = 0;  break;
    }
}

void Car::update(float dt) noexcept {
    float dx = (float)(direction_X);
    float dy = (float)(direction_Y);
    float len = std::sqrt(dx * dx + dy * dy);

    if (len >= EPSILON) {  // para normalizar asi evito que se divida por 0
        dx /= len;
        dy /= len;
    }

    pos_X += dx * (float)(base_speed) * dt;
    pos_Y += dy * (float)(base_speed) * dt;

    direction_X = 0;
    direction_Y = 0;
}

Position Car::get_Pose() const noexcept {
    return Position{pos_X, pos_Y};
}

void Car::set_Pose(float x, float y) noexcept {
    pos_X = x;
    pos_Y = y;
}
