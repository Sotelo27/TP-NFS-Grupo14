#include "building_entity.h"
#include <iostream>
#include "../Player/car.h"


BuildingEntity::BuildingEntity(size_t id, b2Body* body) : Entidad(id, body) {}

Entidad::Type BuildingEntity::type() const {
    return Type::Building;
}

void BuildingEntity::apply_damage_to(Car& car, const CollisionInfo& info) {
    float base_damage = 15.f;
    std::cout << "[COLISSION BUILDING] Car ID " << car.get_id() << " choco con un edificio (ID " << get_id() << ")\n";
    car.apply_collision_damage(base_damage, info);
}
