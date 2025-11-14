#ifndef RACE_PARTICIPANT_H
#define RACE_PARTICIPANT_H

#include <cstdint>

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
    uint8_t car_id{0};
};

#endif
