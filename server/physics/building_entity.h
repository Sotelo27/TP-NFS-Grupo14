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
     * Maneja el daño que este edificio debe causar a un auto que lo
     * haya chocado.
     * Auto aplica el daño según el edificio que lo haya chocado
     */
    void apply_damage_to(Car& car, const CollisionInfo& info) override;
};

#endif
