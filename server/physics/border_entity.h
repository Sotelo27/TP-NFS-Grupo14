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
    BorderEntity(size_t id, b2Body* body): Entidad(id, body) {}

    /*
     * Retorna el tipo para el despachador de colisiones
     */
    Type type() const override;

    /*
     * Logica de colisión para bordes, por el momento solo va a imprimir que lo colisiono un auto
     * esto para avisarle al usuario en la consola del servidor (debug)
     */
    void onCollision(Entidad* other) override;
};

#endif
