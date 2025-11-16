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
    uint32_t current_checkpoint{0};
    bool finished{false};
    uint32_t next_checkpoint_idx{0};
};

struct RankInfo {
    uint32_t player_id;
    uint32_t checkpoints_done;
    float distance_to_next_px;
};

#endif
