#ifndef BORDER_ENTITY_H
#define BORDER_ENTITY_H

#include "Entidad.h"

class BorderEntity: public Entidad {
public:
    /*
     * Construye una BorderEntity asociada a un body estatico de Box2D
     * - id: identificador interno de la entidad
     * - body: puntero al cuerpo de Box2D
     */
    BorderEntity(size_t id, b2Body* body);

    /*
     * Retorna el tipo para el despachador de colisiones
     */
    Type type() const override;

    /*
     * Maneja el daño que este borde debe causar a un auto que lo haya chocado
     * Auto aplica el daño segun el borde que lo haya chocado
     */
    void apply_damage_to(Car& car, const CollisionInfo& info) override;
};

#endif
