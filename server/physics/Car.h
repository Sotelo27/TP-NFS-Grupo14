#ifndef CAR_H
#define CAR_H

#include "Entidad.h"
#include "../../common/car_model.h"
#include <algorithm>
#include <cmath>

class Car : public Entidad {
private:
    CarModel spec_;
    float vida_{100.f};

public:
    Car(size_t id, const CarModel& spec, b2Body* body)
        : Entidad(id, body), spec_(spec) {
        if (this->body) {
            this->body->SetBullet(true);
            this->body->SetLinearDamping(spec_.dampingLineal);
            this->body->SetAngularDamping(spec_.dampingAngular);
            this->body->GetUserData().pointer = reinterpret_cast<uintptr_t>(static_cast<Entidad*>(this));
        }
    }

    Type type() const override { return Type::Car; }

    const CarModel& spec() const noexcept { return spec_; }
    float vida() const noexcept { return vida_; }
    void set_vida(float v) noexcept { vida_ = std::max(0.f, v); }


    void apply_input(float throttle, float steer) noexcept {
        if (!this->body) return;
        throttle = std::clamp(throttle, -1.f, 1.f);
        steer    = std::clamp(steer,    -1.f, 1.f);
        apply_force_center(throttle);
        apply_steer(steer);
        if (spec_.velocidadMaxMps > 0.f) {
            cap_speed(spec_.velocidadMaxMps);
        }
    }

private:
    void apply_force_center(float throttle) noexcept {
        if (!this->body || throttle == 0.f) return;
        const float ang = this->body->GetAngle();
        const float fx = throttle * spec_.fuerzaAceleracionN * std::cos(ang);
        const float fy = throttle * spec_.fuerzaAceleracionN * std::sin(ang);
        this->body->ApplyForceToCenter(b2Vec2(fx, fy), true);
    }

    void apply_steer(float steer) noexcept {
        if (!this->body || steer == 0.f) return;
        const float v = speed_mps();
        // No permitir giro en el lugar: requiere algo de velocidad
        if (v < 0.1f) return;
        float k = (v - 0.5f) / 5.0f;
        k = std::clamp(k, 0.2f, 1.0f);
        this->body->ApplyTorque(steer * spec_.torqueGiro * k, true);
    }

    float speed_mps() const noexcept {
        if (!this->body) return 0.f;
        return this->body->GetLinearVelocity().Length();
    }

    void cap_speed(float vmax_mps) noexcept {
        if (!this->body) return;
        b2Vec2 v = this->body->GetLinearVelocity();
        const float speed = v.Length();
        if (speed > vmax_mps && speed > 0.f) {
            v *= (vmax_mps / speed);
            this->body->SetLinearVelocity(v);
        }
    }

public:
    void onCollision(Entidad* other) override {
        if (!other) return;
        if (other->type() == Type::Building || other->type() == Type::Border) {
            set_vida(vida_ - 10.f);
        }
    }
};

#endif
