#ifndef GARAGE_H
#define GARAGE_H

#include <map>
#include <vector>
#include <cstdint>
#include "../../common/car_model.h"

struct CarSlot {
    uint8_t id_car;
    CarModel model;
};

class Garage {
private:
    std::map<uint8_t, CarSlot> cars;

    void initialize_cars();

public:
    Garage();

    /*
     * Obtiene una lista de todos los autos en el garage
     */
    std::vector<CarSlot> get_available_cars();

    /*
     * Reserva un carro por su ID. Retorna true si la reserva fue exitosa,
     * false si el carro no existe
     */
    bool reserve_car(uint8_t car_id);

    /*
     * Obtiene el modelo de un carro por su ID
     * Lanza std::invalid_argument si el ID no existe
     */
    CarModel get_car_model(uint8_t car_id);
};

#endif
