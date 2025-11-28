#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <cstddef>
#include <box2d/box2d.h>

class Car; // forward para prueba daño

struct CollisionInfo {
    b2Vec2 normal_world;
    float impact_intensity;
};

class Entidad {

protected:
    size_t id;
    b2Body* body;

public:
    
    Entidad(size_t id, b2Body* body): id(id), body(body) {}
    virtual ~Entidad() = default;

    enum class Type { Car, Border, Building, Checkpoint };
    size_t get_id() const noexcept { return id; }
    b2Body* get_body() const noexcept { return body; }

    virtual Type type() const = 0;
    virtual void onCollision(Entidad* /*other*/) {}
    virtual void onUpdate(float /*dt*/) {}
    
    virtual void on_collision_with(Entidad& /*other*/, const CollisionInfo& /*info*/){}
    virtual void apply_damage_to(Car& /*car*/, const CollisionInfo& /*info*/){}
};

#endif
