#include "checkpoint_entity.h"
#include <iostream>
#include <utility>

CheckpointEntity::CheckpointEntity(size_t id, b2Body* body, int index, std::string race_id, std::string type)
    : Entidad(id, body),
      index(index),
      race_id(std::move(race_id)),
      type_(std::move(type)) {}

Entidad::Type CheckpointEntity::type() const {
    return Type::Checkpoint;
}

int CheckpointEntity::get_index() const noexcept {
    return index;
}

const std::string& CheckpointEntity::get_race_id() const noexcept {
    return race_id;
}

const std::string& CheckpointEntity::get_cp_type() const noexcept {
    return type_;
}

void CheckpointEntity::onCollision(Entidad* other) {
    if (!other) return;
    if (other->type() == Type::Car) {
        std::cout << "[CheckpointContact] "<< "Car ID " << other->get_id()
                  << " atraveso checkpoint " << get_index()
                  << " (race " << get_race_id()
                  << ", tipo=" << get_cp_type() << ")\n";
    }
}
