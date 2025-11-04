#ifndef RACE_PARTICIPANT_H
#define RACE_PARTICIPANT_H

#include <cstdint>
#include "../common/car_model.h"

// Estado del participante dentro de una carrera
// Ya no guarda la pose. la pose vive en ahora en PhysicsWorld
enum class ParticipantState : uint8_t {
    Active,
    Disconnected,
    Disqualified,
    Finished
};

struct RaceParticipant {
    ParticipantState state{ParticipantState::Active};
    const CarModel* spec{nullptr};
};

#endif
