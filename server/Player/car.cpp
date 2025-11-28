#include "car.h"
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iostream>

constexpr float THROTTLE_DEADZONE = 0.02f;
constexpr float STEER_DEADZONE    = 0.02f;
constexpr float MIN_STEER_SPEED   = 0.15f;

Car::Car(size_t id, const CarModel& spec, b2Body* body)
    : Entidad(id, body), spec_(spec) {
    if (auto* b = this->body) {
        b->SetBullet(true);
        b->SetLinearDamping(spec_.dampingLineal);
        b->SetAngularDamping(spec_.dampingAngular);
    // Asociar puntero a Entidad en el userData del body // INTENTE CON SETUSERDATA Y NO ME FUNCIONO
    b->GetUserData().pointer = reinterpret_cast<uintptr_t>(this); // aqui unimos las dos referencias, entonces Car ya se asocia su BODY y en BOdy asociamos el Car
                              // entonces en colisiones podemos recuperar el Car desde el Body, asi evitamos buscar en mapas
    }
    set_vida(spec_.life);
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
    vida_ = v;
}

void Car::apply_input(float throttle, float steer) noexcept {
    if (!body) return;

    throttle = std::clamp(throttle, -1.f, 1.f);
    steer    = std::clamp(steer,    -1.f, 1.f);

    // cancelación de deriva lateral
    apply_lateral_grip();
    const float v = speed_mps();

    // girar solo si hay algo de velocidad O estamos acelerando
    if (std::abs(steer) > STEER_DEADZONE &&
        (v > MIN_STEER_SPEED || std::abs(throttle) > THROTTLE_DEADZONE)) {
        apply_steer(steer);
    } else {
        // si estamos casi parados, mata cualquier giro residual
        if (v <= MIN_STEER_SPEED && std::abs(body->GetAngularVelocity()) > 0.0f) {
            body->SetAngularVelocity(0.0f);
        }
    }

    if (std::abs(throttle) > THROTTLE_DEADZONE) {
        apply_force_center(throttle);
    }

    if (spec_.velocidadMaxMps > 0.f) {
        cap_speed(spec_.velocidadMaxMps);
    }
}


void Car::apply_lateral_grip() noexcept {
    if (!body) return;

    const float m   = body->GetMass();
    const float ang = body->GetAngle();
    const b2Vec2 vel = body->GetLinearVelocity();

    const b2Vec2 right(-std::sin(ang), std::cos(ang));
    const float vlat = b2Dot(vel, right);
    const float v    = vel.Length();
    constexpr float GRIP_MIN = 0.45f;
    constexpr float GRIP_MAX = 0.85f;
    const float k = std::clamp(v / 9.1f, 0.0f, 1.0f);
    const float grip = GRIP_MIN + (GRIP_MAX - GRIP_MIN) * k;
    const b2Vec2 J = (-grip * m * vlat) * right;
    body->ApplyLinearImpulseToCenter(J, true);
}

void Car::apply_force_center(float throttle) noexcept {
    if (!body) return;

    float t = std::clamp(throttle, -1.f, 1.f);
    if (t == 0.f) return;

    const float v    = speed_mps();
    const float vmax = std::max(0.1f, spec_.velocidadMaxMps);

    float accel_target = spec_.fuerzaAceleracionN;

    // escala de fuerza  segun si esta cerca de la velocidad maxima
    const float x = std::clamp(v / vmax, 0.0f, 1.0f);
    const float scale = 1.0f - x;

    // fuerza
    float F = t * accel_target * scale;

    const float ang = body->GetAngle();
    b2Vec2 forward(std::cos(ang), std::sin(ang));

    body->ApplyForceToCenter(F * forward, true);
}

void Car::apply_steer(float steer) noexcept {
    if (!body) return;

    steer = std::clamp(steer, -1.f, 1.f);
    if (std::fabs(steer) < STEER_DEADZONE) return;

    const float v   = speed_mps();
    const float ang = body->GetAngle();

    // --- invertir sentido si esta marcha atras---
    const b2Vec2 forward(std::cos(ang), std::sin(ang));
    const float vlong = b2Dot(body->GetLinearVelocity(), forward);
    float steerEff = (vlong < -0.2f) ? -steer : steer;

    // potencia base de giro
    float turn_power = spec_.torqueGiro / 5.0f;

    // a mayor velocidad, menor giro (curva suavizada)
    float speed_factor = std::clamp(v / spec_.velocidadMaxMps, 0.0f, 1.0f);

    // minimo giro incluso a alta velocidad
    float omega_min = 0.8f;

    // giro interpolado
    float omega = turn_power * (1.0f - 0.5f * speed_factor) + omega_min;

    // aplicar giro final
    body->SetAngularVelocity(steerEff * omega);
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

void Car::on_collision_with(Entidad& other, const CollisionInfo& info) {
    other.apply_damage_to(*this, info);
}

void Car::apply_damage_to(Car& other_car, const CollisionInfo& info) {
    // choque auto-auto: ambos se dañan
    float base_damage = 5.f;
    other_car.apply_collision_damage(base_damage, info);
    this->apply_collision_damage(base_damage, info);
}

void Car::apply_collision_damage(float base_damage, const CollisionInfo& info) {
    if (!body) return;

    // Dirección del auto
    b2Vec2 forward = body->GetWorldVector(b2Vec2(0.f, 1.f)); 
    float alignment = std::fabs(b2Dot(forward, -info.normal_world));
    alignment = std::clamp(alignment, 0.f, 1.f);

    float impact_factor = info.impact_intensity / 10.0f;
    impact_factor = std::clamp(impact_factor, 0.2f, 2.0f);

    float direction_factor = 0.3f + 0.7f * alignment;

    float damage = base_damage * direction_factor * impact_factor;

    set_vida(vida_ - damage);
}
