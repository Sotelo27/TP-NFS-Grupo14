#ifndef GARAGE_H
#define GARAGE_H

#include <map>
#include <mutex>
#include <vector>
#include "../../common/car_model.h"

struct CarSlot {
    uint8_t id_car;
    CarModel model;
    bool reservado = false;
};

class Garage {
private:
    std::map<uint8_t, CarSlot> cars;
    std::mutex m;

    void initialize_cars();

public:
    Garage();

    /*
     * Obtiene una lista de los carros disponibles (no reservados)    
     */
    std::vector<CarSlot> get_available_cars();

    /*
     * Reserva un carro por su ID. Retorna true si la reserva fue exitosa,
     * false si el carro ya estaba reservado
     */
    bool reserve_car(uint8_t car_id);

    /*
     * Libera un carro previamente reservado por su ID
     */
    void release_car(uint8_t car_id);

    /*
     * Obtiene el modelo de un carro por su ID
     * Lanza std::invalid_argument si el ID no existe
     */
    CarModel get_car_model(uint8_t car_id);

    /*
     * Verifica si un carro está disponible (no reservado)
     */
    bool is_car_available(uint8_t car_id);
};

#endif
