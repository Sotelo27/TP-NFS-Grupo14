#ifndef CAR_H
#define CAR_H

#include <cstdint>
#include <string>

#include "../common/base_protocol.h"

struct Position {
    float x;
    float y;
};

class Car {
private:
    std::string model;
    uint16_t base_speed;
    uint16_t base_acceleration;
    uint16_t base_mass;
    uint16_t base_control;
    uint16_t base_health;
    uint16_t current_health;
    float pos_X;
    float pos_Y;
    int direction_X;
    int direction_Y;

public:
    Car(std::string model,
         uint16_t base_speed,
         uint16_t base_acceleration,
         uint16_t base_mass,
         uint16_t base_control,
         uint16_t base_health);

    void move(Movement mv) noexcept;
    void update(float dt) noexcept;

    Position get_Pose() const noexcept;
    const std::string& get_Model() const noexcept;
    void set_Pose(float x, float y) noexcept;
};

#endif
