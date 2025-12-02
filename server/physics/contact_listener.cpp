#include "contact_listener.h"
#include "checkpoint_entity.h"
#include "checkpoint_event.h"
#include "damage_event.h"
#include "../Player/car.h"

#include <algorithm>

namespace {
inline Entidad* entidadFromFixture(const b2Fixture* fix) noexcept {
    if (!fix) return nullptr;
    b2Body* body = const_cast<b2Body*>(fix->GetBody());
    if (!body) return nullptr;
    return reinterpret_cast<Entidad*>(body->GetUserData().pointer);
}
}


void ContactListener::BeginContact(b2Contact* contact) {
    if (!contact) return;

    Entidad* a = entidadFromFixture(contact->GetFixtureA());
    Entidad* b = entidadFromFixture(contact->GetFixtureB());
    if (!a || !b) return;

    handle_checkpoint_contact(a, b);
}

void ContactListener::EndContact(b2Contact* contact) {
    if (!contact) return;
    damaged_contacts_.erase(contact);
}

void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
    if (!contact || !impulse) return;

    Entidad* a = entidadFromFixture(contact->GetFixtureA());
    Entidad* b = entidadFromFixture(contact->GetFixtureB());
    if (!a || !b) return;

    // Normal del contacto
    b2WorldManifold wm;
    contact->GetWorldManifold(&wm);
    b2Vec2 normalAB = wm.normal;

    // Severidad del impacto: usamos el impulso normal (primer punto)
    //fuerza del impacto
    float normalImpulse = (impulse->normalImpulses[0] > 0.0f)
                            ? impulse->normalImpulses[0]
                            : 0.0f;

    // No proceses daño si el golpe es demasiado suave
    if (normalImpulse < MIN_DAMAGE_IMPULSE) {
        return;
    }

    // Evitar aplicar daño cada frame mientras el contacto persiste
    if (damaged_contacts_.find(contact) != damaged_contacts_.end()) {
        return;
    }

    CollisionInfo info_calculate_collision_A{ -normalAB, normalImpulse };
    CollisionInfo info_calculate_collision_B{  normalAB, normalImpulse };

    a->on_collision_with(*b, info_calculate_collision_A);
    b->on_collision_with(*a, info_calculate_collision_B);

    // Registrar evento de daño
    if (a->type() == Entidad::Type::Car) {
        auto* car = static_cast<Car*>(a);
        damage_events_.emplace_back(car->player_id(), normalImpulse);
    }
    if (b->type() == Entidad::Type::Car) {
        auto* car = static_cast<Car*>(b);
        damage_events_.emplace_back(car->player_id(), normalImpulse);
    }

    // Marcar el contacto como ya dañado hasta que se libere (EndContact)
    damaged_contacts_.insert(contact);
}


void ContactListener::handle_checkpoint_contact(Entidad* a, Entidad* b) {
    Entidad*            ent_car = nullptr;
    CheckpointEntity*   ent_cp  = nullptr;

    if (a->type() == Entidad::Type::Car && b->type() == Entidad::Type::Checkpoint) {
        ent_car = a;
        ent_cp  = static_cast<CheckpointEntity*>(b);
    } else if (b->type() == Entidad::Type::Car && a->type() == Entidad::Type::Checkpoint) {
        ent_car = b;
        ent_cp  = static_cast<CheckpointEntity*>(a);
    } else {
        return;
    }

    events_.emplace_back(ent_car->get_id(), ent_cp->get_race_id(), ent_cp->get_index());
}

std::vector<CheckpointEvent> ContactListener::consume_checkpoint_events() {
    std::vector<CheckpointEvent> out;
    out.swap(events_);
    return out;
}

std::vector<DamageEvent> ContactListener::consume_damage_events() {
    std::vector<DamageEvent> out;
    out.swap(damage_events_);
    return out;
}

