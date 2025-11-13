#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "../../common/car_model.h"
#include "../../common/dto/movement.h"

class Player {
private:
    size_t id = 0;
    std::string name;
    uint8_t car_id = 0;
    CarModel car;

public:
    explicit Player(size_t id, std::string name, CarModel car);
    Player(size_t id);

    size_t get_Id() const;
    const std::string& get_name() const;
    void set_name(std::string name);

    uint8_t get_car_id() const noexcept { return car_id; }
    void set_car_id(uint8_t id) noexcept { car_id = id; }

    const CarModel& get_car_model() const noexcept { return car; }
    void set_car_model(const CarModel& c) noexcept { car = c; }
};

#endif
