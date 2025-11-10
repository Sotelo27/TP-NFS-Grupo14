#include "contact_listener.h"

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
}
