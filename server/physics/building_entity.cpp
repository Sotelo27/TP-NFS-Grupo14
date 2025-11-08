#include "building_entity.h"
#include <iostream>


BuildingEntity::BuildingEntity(size_t id, b2Body* body) : Entidad(id, body) {}

Entidad::Type BuildingEntity::type() const {
    return Type::Building;
}

void BuildingEntity::onCollision(Entidad* other) {
    if (other && other->type() == Type::Car) {
        std::cout << "Car ID " << other->get_id() << " chocó con un edificio (ID " << get_id() << ")\n";
    }
}
