#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>

// Modelo de auto con parametros fisicos defautl usados
struct CarModel {
    std::string modelo;
    // Ajustes
    float masaKg{1200.f};
    float fuerzaAceleracionN{500.f};
    float torqueGiro{10.0f};
    float velocidadMaxMps{7.1f};
    float dampingLineal{0.70f};
    float dampingAngular{5.5f};
};

#endif
