#include "contact_listener.h"
#include "checkpoint_entity.h"
#include "checkpoint_event.h"

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

    a->onCollision(b);
    b->onCollision(a);

    handle_checkpoint_contact(a, b);
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

