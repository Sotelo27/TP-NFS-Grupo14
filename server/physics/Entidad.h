#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <cstddef>
#include <box2d/box2d.h>

class Entidad {
public:
    enum class Type { Car, Border, Building, Unknown };

protected:
    size_t id;
    b2Body* body;

public:
    Entidad(size_t id, b2Body* body): id(id), body(body) {}
    virtual ~Entidad() = default;

    size_t get_id() const noexcept { return id; }
    b2Body* get_body() const noexcept { return body; }

    virtual Type type() const = 0;
    virtual void onCollision(Entidad* /*other*/) {}
    virtual void onUpdate(float /*dt*/) {}
};

#endif
