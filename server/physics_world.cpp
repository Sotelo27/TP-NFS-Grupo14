#include "physics_world.h"

PhysicsWorld::PhysicsWorld() : world(b2Vec2(0.0f, 0.0f)) {}

void PhysicsWorld::create_body_with_spec(size_t id, int16_t x_units, int16_t y_units, const CarModel& spec) {
    destroy_body(id);

    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.fixedRotation = false;
    def.position.Set(toMeters(x_units), toMeters(y_units));
    b2Body* body = world.CreateBody(&def);

    b2PolygonShape box;
    float halfW = 0.5f; // TODO: derivar de spec si esque tenemos las dimensiones reales del auto
    float halfL = 0.5f;
    box.SetAsBox(halfW, halfL);

    b2FixtureDef fdef;
    fdef.shape = &box;
    fdef.density = (spec.masaKg > 0.f) ? spec.masaKg / 1000.f : 1.f; // densidad por ahora.
    fdef.friction = 0.9f;
    fdef.restitution = 0.0f;
    body->CreateFixture(&fdef);

    body->SetLinearDamping(spec.dampingLineal);
    body->SetAngularDamping(spec.dampingAngular);

    bodies[id] = body;
}

void PhysicsWorld::destroy_body(size_t id) {
    auto it = bodies.find(id);
    if (it != bodies.end()) {
        world.DestroyBody(it->second);
        bodies.erase(it);
    }
}

void PhysicsWorld::step(float dt) {
    // Iteraciones razonables por ahora
    world.Step(dt, 6, 2);
}

void PhysicsWorld::apply_force_center(size_t id, float fx, float fy) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return;
    it->second->ApplyForceToCenter(b2Vec2(fx, fy), true);
}

void PhysicsWorld::apply_torque(size_t id, float torque) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return;
    it->second->ApplyTorque(torque, true);
}

float PhysicsWorld::get_angle(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return 0.f;
    return it->second->GetAngle();
}

Pose PhysicsWorld::get_pose(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return Pose{0, 0, 0.f};
    b2Vec2 p = it->second->GetPosition();
    return Pose{toUnits(p.x), toUnits(p.y), it->second->GetAngle()};
}

void PhysicsWorld::cap_linear_speed(size_t id, float max_mps) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return;
    b2Vec2 v = it->second->GetLinearVelocity();
    float speed = v.Length();
    if (speed > max_mps && speed > 0.f) {
        v *= (max_mps / speed);
        it->second->SetLinearVelocity(v);
    }
}

float PhysicsWorld::get_linear_speed(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return 0.f;
    b2Vec2 v = it->second->GetLinearVelocity();
    return v.Length();
}
