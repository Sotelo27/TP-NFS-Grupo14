#ifndef BUILDING_ENTITY_H
#define BUILDING_ENTITY_H

#include "Entidad.h"

class BuildingEntity: public Entidad {
public:
    /*
     * Construye una BuildingEntity asociada a un body estático de Box2D.
     *
     * - id: identificador interno de la entidad (no tiene por qué coincidir
     *       con el id de jugador; se usa para trazas o depuración).
     * - body: puntero al cuerpo de Box2D ya creado y perteneciente al mundo.
     */
    BuildingEntity(size_t id, b2Body* body);

    /*
     * Retorna el tipo de entidad para que el listener pueda despachar
     * las colisiones adecuadamente
     */
    Type type() const override;
    
    /*
     * Logica de colisión para edificios, por el momento solo va sprimir que lo colosiono un auto 
     * con su mensaje de colision ID
     */
    void onCollision(Entidad* other) override;
};

#endif
