#include "car.h"
#include <cmath>
#include <algorithm>

Car::Car(size_t id, const CarModel& spec, b2Body* body)
    : Entidad(id, body), spec_(spec) {
    if (auto* b = this->body) {
        b->SetBullet(true);
        b->SetLinearDamping(spec_.dampingLineal);
        b->SetAngularDamping(spec_.dampingAngular);
        b->GetUserData().pointer =
            reinterpret_cast<uintptr_t>(static_cast<Entidad*>(this));
    }
}

Entidad::Type Car::type() const {
    return Entidad::Type::Car;
}

const CarModel& Car::get_spec() const noexcept {
    return spec_;
}

float Car::get_vida() const noexcept {
    return vida_;
}

void Car::set_vida(float v) noexcept {
    vida_ = std::max(0.f, v);
}

void Car::apply_input(float throttle, float steer) noexcept {
    if (!this->body){
        return;
    }

    throttle = std::clamp(throttle, -1.f, 1.f);
    steer    = std::clamp(steer,    -1.f, 1.f);

    apply_force_center(throttle);
    apply_steer(steer);

    if (spec_.velocidadMaxMps > 0.f) {
        cap_speed(spec_.velocidadMaxMps);
    }
}

void Car::apply_force_center(float throttle) noexcept {
    b2Body* b = this->body;
    if (!b || throttle == 0.f){
        return;
    }

    const float ang = b->GetAngle();
    const float fx = throttle * spec_.fuerzaAceleracionN * std::cos(ang);
    const float fy = throttle * spec_.fuerzaAceleracionN * std::sin(ang);
    b->ApplyForceToCenter(b2Vec2(fx, fy), true);
}

void Car::apply_steer(float steer) noexcept {
    b2Body* b = this->body;
    if (!b || steer == 0.f){
        return;
    }

    const float v = speed_mps();
    // No permitir giro en el lugar: requiere algo de velocidad
    if (v < 0.1f){
        return;
    }

    float k = (v - 0.5f) / 5.0f;
    k = std::clamp(k, 0.2f, 1.0f);

    b->ApplyTorque(steer * spec_.torqueGiro * k, true);
}

float Car::speed_mps() const noexcept {
    const b2Body* b = this->body;
    if (!b) return 0.f;
    return b->GetLinearVelocity().Length();
}

void Car::cap_speed(float vmax_mps) noexcept {
    b2Body* b = this->body;
    if (!b){
        return;
    }

    b2Vec2 v = b->GetLinearVelocity();
    const float speed = v.Length();

    if (speed > vmax_mps && speed > 0.f) {
        v *= (vmax_mps / speed);
        b->SetLinearVelocity(v);
    }
}

void Car::onCollision(Entidad* other) {
    if (!other){
        return;
    }
    if (other->type() == Entidad::Type::Building || other->type() == Entidad::Type::Border) {
        set_vida(vida_ - 10.f);
    }
}
