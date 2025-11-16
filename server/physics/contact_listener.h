#ifndef PHYSICS_CONTACT_LISTENER_H
#define PHYSICS_CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include "Entidad.h"

class PhysicsWorld;

class ContactListener: public b2ContactListener {
private:
    PhysicsWorld* world;

public:

    explicit ContactListener(PhysicsWorld* world);

    /*
     * Notificación de colisión. Se usa BeginContact para aplicar
     * penalización simple sin depender del impulso de colisión.
     * En el futuro podría migrarse a PostSolve para un daño
     * proporcional al impacto.
     */
    void BeginContact(b2Contact* contact) override;

    /*
     * Maneja el evento de cruce de checkpoint entre un Car y un Checkpoint
     */
    void handle_checkpoint_contact(Entidad* a, Entidad* b);
};

#endif
