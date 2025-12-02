#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>
#include <vector>
#include "enum/car_improvement.h"

// Modelo de auto con parametros fisicos defautl usados
struct CarModel {
    std::string modelo;
    // Ajustes
    float life{100.f};
    float masaKg{1200.f};
    float fuerzaAceleracionN{250.f};
    float torqueGiro{8.5f};
    float velocidadMaxMps{5.1f};
    float dampingLineal{0.70f};
    float dampingAngular{5.5f};
    std::vector<CarImprovement> improvements;
};

namespace car_factory {

    inline CarModel common_green_car()      { return {"green", 200.f, 800.f, 80.f, 7.0f,  50.0f, 0.2f, 5.5f, {}}; }
    inline CarModel red_car()      { return {"red_car",  200.f, 850.f, 90.f, 8.0f,  55.0f, 0.22f, 5.3f, {}}; }
    inline CarModel red_sport_car()  { return {"red_sport_car",200.f, 900.f, 110.f, 9.5f,  60.0f, 0.2f, 4.8f, {}}; }
    inline CarModel special_car()      { return {"special_car",200.f,880.f, 100.f, 8.8f,  56.0f, 0.22f, 5.2f, {}}; }
    inline CarModel four_by_four_convertible()        { return {"four_by_four_convertible", 200.f, 1200.f, 85.f, 7.2f,  48.0f, 0.28f, 6.3f, {}}; }
    inline CarModel pickup_truck()       { return {"pickup_truck", 200.f, 1350.f, 75.f, 7.0f,  45.0f, 0.30f, 6.5f, {}}; }
    inline CarModel limousine()     { return {"limousine", 200.f, 1500.f, 65.f, 6.8f,  42.0f, 0.32f, 6.8f, {}}; }
}

#endif
