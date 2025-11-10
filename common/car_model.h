#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>

// Modelo de auto con parametros fisicos defautl usados
struct CarModel {
    std::string modelo;
    // Ajustes
    float masaKg{1200.f};
    float fuerzaAceleracionN{900.f};
    float torqueGiro{15.0f};
    float velocidadMaxMps{4.8f};
    float dampingLineal{2.2f};
    float dampingAngular{5.2f};
};

#endif
