#ifndef PHYSICS_CONTACT_LISTENER_H
#define PHYSICS_CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include "Entidad.h"

class ContactListener: public b2ContactListener {
public:
    /*
     * Notificación de colisión. Se usa BeginContact para aplicar
     * penalización simple sin depender del impulso de colisión.
     * En el futuro podría migrarse a PostSolve para un daño
     * proporcional al impacto.
     */
    void BeginContact(b2Contact* contact) override;
};

#endif
