#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>

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
};

namespace car_factory {

    inline CarModel common_green_car()      { return {"green", 200.f, 800.f, 80.f, 7.0f,  50.0f, 0.2f, 5.5f}; }
    inline CarModel red_car()      { return {"red_car",  200.f, 1100.f, 270.f, 9.0f,  5.6f, 0.68f, 5.0f}; }
    inline CarModel red_sport_car()  { return {"red_sport_car",200.f, 1050.f, 300.f, 9.5f,  6.2f, 0.66f, 4.8f}; }
    inline CarModel special_car()      { return {"special_car",200.f,1120.f, 255.f, 8.7f,  5.4f, 0.70f, 5.2f}; }
    inline CarModel four_by_four_convertible()        { return {"four_by_four_convertible", 200.f, 1500.f, 240.f, 7.2f,  5.0f, 0.78f, 6.0f}; }
    inline CarModel pickup_truck()       { return {"pickup_truck", 200.f, 1450.f, 235.f, 7.0f,  4.9f, 0.80f, 6.2f}; }
    inline CarModel limousine()     { return {"limousine", 200.f, 1550.f, 150.f, 7.3f,  4.0f, 0.79f, 6.1f}; }
}

#endif
