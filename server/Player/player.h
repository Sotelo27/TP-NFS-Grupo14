#ifndef PLAYER_H
#define PLAYER_H

#include <string>

#include "../../common/car_model.h"
#include "../../common/dto/movement.h"

class Player {
private:
    size_t id = 0;
    std::string name;
    CarModel car;

public:
    explicit Player(size_t id, std::string name, CarModel car);
    Player(size_t id);

    size_t get_Id() const;
    const std::string& get_name() const;
    void set_name(std::string name);

    // Acceso al modelo (parametros fisicos) del auto
    const CarModel& get_car_model() const noexcept { return car; }
    void set_car_model(const CarModel& c) noexcept { car = c; }
};

#endif
