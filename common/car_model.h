#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>

// Modelo de auto con parametros fisicos defautl usados
struct CarModel {
    std::string modelo;
    // Ajustes
    float masaKg{1200.f};
    float fuerzaAceleracionN{250.f};
    float torqueGiro{8.5f};
    float velocidadMaxMps{5.1f};
    float dampingLineal{0.70f};
    float dampingAngular{5.5f};
};

namespace car_factory {

    inline CarModel common_green_car()      { return {"Sedán",      1250.f, 250.f, 8.0f,  5.2f, 0.72f, 5.5f}; }
    inline CarModel red_car()      { return {"Coupé",      1100.f, 270.f, 9.0f,  5.6f, 0.68f, 5.0f}; }
    inline CarModel red_sport_car()  { return {"Deportivo",  1050.f, 300.f, 9.5f,  6.2f, 0.66f, 4.8f}; }
    inline CarModel special_car()      { return {"Hatch",      1120.f, 255.f, 8.7f,  5.4f, 0.70f, 5.2f}; }
    inline CarModel four_by_four_convertible()        { return {"SUV",        1500.f, 240.f, 7.2f,  5.0f, 0.78f, 6.0f}; }
    inline CarModel pickup_truck()       { return {"Jeep",       1450.f, 235.f, 7.0f,  4.9f, 0.80f, 6.2f}; }
    inline CarModel limousine()     { return {"Pickup",     1550.f, 245.f, 7.3f,  5.0f, 0.79f, 6.1f}; }
}

#endif
