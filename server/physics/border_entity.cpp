#include "border_entity.h"
#include <iostream>
#include "../Player/car.h"


BorderEntity::BorderEntity(size_t id, b2Body* body): Entidad(id, body) {}

Entidad::Type BorderEntity::type() const {
    return Type::Border;
}

void BorderEntity::apply_damage_to(Car& car, const CollisionInfo& info) {
    float base_damage = 10.f; // daño contra borde
    std::cout << "[COLISSION BORDER] Car ID " << car.get_id()  << " choco con un borde (ID " << get_id() << ")\n";
    car.apply_collision_damage(base_damage, info);
}
