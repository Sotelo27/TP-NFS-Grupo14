#ifndef PHYSICS_CONTACT_LISTENER_H
#define PHYSICS_CONTACT_LISTENER_H

#include <box2d/box2d.h>
#include <vector>
#include <unordered_set>
#include "Entidad.h"
#include "checkpoint_event.h"
#include "damage_event.h"

#define MIN_DAMAGE_IMPULSE 0.75f

class ContactListener: public b2ContactListener {
private:
    std::vector<CheckpointEvent> events_;
    std::vector<DamageEvent> damage_events_;
    std::unordered_set<const b2Contact*> damaged_contacts_;

public:

    ContactListener() = default;

    /*
     * Notificación de contacto inicial, solo detecta la colisión.
     * su funcion es mas para detectar eventos como checkpoints.
     */
    void BeginContact(b2Contact* contact) override;

    /*
     * Cuando finaliza un contacto, se limpia el registro para permitir daño
     * si en el futuro vuelven a chocar nuevamente.
     */
    void EndContact(b2Contact* contact) override;

    /*
     * Notificación de colisión con información de impulso.
     * Permite manejar la severidad del impacto
     */
    void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

    /*
     * Maneja el evento de cruce de checkpoint entre un Car y un Checkpoint
     */
    void handle_checkpoint_contact(Entidad* a, Entidad* b);

    /*
     * Consume y limpia los eventos de cruce de checkpoint registrados
     */
    std::vector<CheckpointEvent> consume_checkpoint_events();

    // Consume y limpia eventos de daño registrados
    std::vector<DamageEvent> consume_damage_events();
};

#endif
