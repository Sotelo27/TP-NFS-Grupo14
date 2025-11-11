#ifndef CHECKPOINT_ENTITY_H
#define CHECKPOINT_ENTITY_H

#include "Entidad.h"
#include <string>

class CheckpointEntity : public Entidad {
private:
    int index;
    std::string race_id;
    std::string type_;

public:
    CheckpointEntity(size_t id, b2Body* body, int index, std::string race_id, std::string type);

    Type type() const override;

    int get_index() const noexcept;
    const std::string& get_race_id() const noexcept;
    const std::string& get_cp_type() const noexcept;

    void onCollision(Entidad* other) override;
};

#endif
