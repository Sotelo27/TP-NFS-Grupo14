#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "../common/base_protocol.h"
#include "car.h"

class Player {
private:
    size_t id = 0;
    std::string name;
    Car car;

public:
    explicit Player(size_t id, std::string name, Car car);

    void execute_movement(Movement mv);
    void update(float dt) noexcept;

    size_t get_Id() const;
    const std::string& get_name() const;
    Position get_Pose() const;
};

#endif
