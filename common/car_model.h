#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>

// Modelo de auto con parametros fisicos defautl usados
struct CarModel {
    std::string modelo;
    // Ajustes
    float masaKg{1200.f};
    float fuerzaAceleracionN{1800.f};
    float torqueGiro{170.0f};
    float velocidadMaxMps{6.2f};
    float dampingLineal{2.2f};
    float dampingAngular{2.0f};
};

#endif
