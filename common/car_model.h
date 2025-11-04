#ifndef CAR_MODEL_H
#define CAR_MODEL_H
#include <string>

// Modelo de auto con parametros fisicos defautl usados
struct CarModel {
    std::string modelo;
    // Ajustes
    float masaKg{1200.f};
    float fuerzaAceleracionN{1800.f};
    float torqueGiro{32.0f};
    float velocidadMaxMps{12.0f};
    float dampingLineal{3.5f};
    float dampingAngular{2.5f};
};

#endif
