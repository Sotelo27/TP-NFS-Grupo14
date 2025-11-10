#include "car.h"
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iostream>

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

    apply_lateral_grip();

    // Giro SOLO con aceleracion
    if (std::abs(throttle) > 0.02f ) {
        apply_steer(steer);
    }
    // const float currentSpeed = speed_mps();
    // if (std::abs(throttle) > 0.02f || currentSpeed > 0.35f) {
    //     apply_steer(steer);
    // }
    apply_force_center(throttle);

    if (spec_.velocidadMaxMps > 0.f) {
        cap_speed(spec_.velocidadMaxMps);
    }
}

b2Vec2 Car::lateral_velocity() const noexcept {
    if (!body) return b2Vec2_zero;
    const float ang = body->GetAngle();
    // Eje lateral (derecha)
    b2Vec2 right(-std::sin(ang), std::cos(ang));
    b2Vec2 v = body->GetLinearVelocity();
    float vlat = b2Dot(v, right);
    return vlat * right;
}

void Car::apply_lateral_grip() noexcept {
    if (!body) return;

    b2Vec2 vlat = lateral_velocity();
    // fuerza que cancela el movimiento lateral
    // Bajamos el grip para permitir un leve derrape controlado
    const float grip = 0.75f;
    const float mass = body->GetMass();

    // impulso opuesto al deslizamiento lateral
    b2Vec2 impulse = -grip * mass * vlat;
    body->ApplyLinearImpulseToCenter(impulse, true);
}


void Car::apply_force_center(float throttle) noexcept {
    b2Body* b = this->body;
    if (!b) return;

    float t = std::clamp(throttle, -1.f, 1.f);
    if (t == 0.f) return;

    const float v = speed_mps();
    const float vmax = spec_.velocidadMaxMps;

    const float x = (vmax > 0.f) ? std::clamp(v / vmax, 0.0f, 1.0f) : 0.0f;
    const float scale = std::clamp(1.0f - (x * x), 0.30f, 1.0f);

    const float ang = b->GetAngle();
    const float fx = t * spec_.fuerzaAceleracionN * scale * std::cos(ang);
    const float fy = t * spec_.fuerzaAceleracionN * scale * std::sin(ang);
    b->ApplyForceToCenter(b2Vec2(fx, fy), true);
}

void Car::apply_steer(float steer) noexcept {
    b2Body* b = this->body;
    if (!b || steer == 0.f){
        return;
    }

    // Invertir el sentido del giro cuando el auto se desplaza hacia atrás
    b2Vec2 lin = b->GetLinearVelocity();
    const float ang = b->GetAngle();
    b2Vec2 forward(std::cos(ang), std::sin(ang));
    const float vlong = b2Dot(lin, forward);

    float steerEff = steer;
    if (vlong < -0.2f) {
        steerEff = -steer;
    }

    const float torqueFinal = spec_.torqueGiro;
    b->ApplyTorque(steerEff * torqueFinal, true);
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
