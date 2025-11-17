#ifndef PHYSICS_CONTACT_LISTENER_H
#define PHYSICS_CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include <vector>
#include "Entidad.h"
#include "checkpoint_event.h"

class ContactListener: public b2ContactListener {
private:
    std::vector<CheckpointEvent> events_;

public:

    ContactListener() = default;

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

    /*
     * Consume y limpia los eventos de cruce de checkpoint registrados
     */
    std::vector<CheckpointEvent> consume_checkpoint_events();
};

#endif
