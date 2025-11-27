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
    const float m    = body->GetMass();

    float accel_target = spec_.fuerzaAceleracionN;

    // escala de fuerza  segun si esta cerca de la velocidad maxima
    const float x = std::clamp(v / vmax, 0.0f, 1.0f);
    const float scale = 1.0f - x;

    // fuerza
    float F = t * m * accel_target * scale;

    const float ang = body->GetAngle();
    b2Vec2 forward(std::cos(ang), std::sin(ang));

    body->ApplyForceToCenter(F * forward, true);
}


void Car::apply_steer(float steer) noexcept {
    if (!body || steer == 0.f) return;

    const float v   = speed_mps();
    const float ang = body->GetAngle();

    // invertir sentido si vas en reversa
    const b2Vec2 forward(std::cos(ang), std::sin(ang));
    const float vlong = b2Dot(body->GetLinearVelocity(), forward);
    float steerEff = (vlong < -0.2f) ? -steer : steer;

    // agresividad desde el modelo
    const float steerFactor = std::clamp(spec_.torqueGiro / 10.0f, 0.6f, 1.6f);

    // ω base: alta a baja velocidad, menor a alta
    const float k = std::clamp(v / std::max(0.1f, spec_.velocidadMaxMps), 0.0f, 1.0f);
    const float omega_lo_base = 3.6f;
    const float omega_hi_base = 1.6f;
    float omega_max = (omega_lo_base + (omega_hi_base - omega_lo_base) * k) * steerFactor;

    if (v < 0.20f) {
        const float pivot_min = 4.8f * steerFactor;
        omega_max = std::max(omega_max, pivot_min);

        // reducir traslación para que no “empuje” contra el muro
        b2Vec2 lv = body->GetLinearVelocity();
        body->SetLinearVelocity(0.85f * lv);
        if (lv.Length() < 0.03f) {
            const float m = body->GetMass();
            const b2Vec2 impulse = (0.25f * m) * -forward;
            body->ApplyLinearImpulseToCenter(impulse, true);
        }
    }

    const float omega_des = steerEff * omega_max;
    const float omega_cur = body->GetAngularVelocity();
    const float I = body->GetInertia();

    // impulso angular para alcanzar ω deseada
    const float J = I * (omega_des - omega_cur);
    body->ApplyAngularImpulse(J, true);
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
    const float oldVida = vida_;
    if (other->type() == Entidad::Type::Building || other->type() == Entidad::Type::Border) {
        const float damage = 10.f;
        set_vida(vida_ - damage);
        std::cout << "[Collision] Car " << id
                  << " vs " << (other->type() == Entidad::Type::Building ? "Building" : "Border")
                  << " | vida: " << oldVida << " -> " << vida_ << " (-" << (oldVida - vida_) << ")\n";
        return;
    }
    if (other->type() == Entidad::Type::Car) {
        const float damage = 5.f;
        set_vida(vida_ - damage);
        std::cout << "[Collision] Car " << id
                  << " vs Car " << other->get_id()
                  << " | vida: " << oldVida << " -> " << vida_ << " (-" << (oldVida - vida_) << ")\n";
        return;
    }
}
