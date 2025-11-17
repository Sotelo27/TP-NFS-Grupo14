#include "border_entity.h"
#include <iostream>


BorderEntity::BorderEntity(size_t id, b2Body* body): Entidad(id, body) {}

Entidad::Type BorderEntity::type() const {
    return Type::Border;
}

void BorderEntity::onCollision(Entidad* other) {
    if (other && other->type() == Type::Car) {
        std::cout << "[COLISSION BORDER] Car ID " << other->get_id()  << " choco con un borde (ID " << get_id() << ")\n";
    }
}
