#ifndef RACE_INFO_H
#define RACE_INFO_H

#include <cstdint>
#include <vector>
#include <string>

// Estado del participante dentro de una carrera
enum class ParticipantState : uint8_t {
    Active,
    Disconnected,
    Disqualified,
    Finished,
    Crashed,
    TimeOut
};

struct RaceParticipant {
    ParticipantState state{ParticipantState::Active};
    uint8_t car_id{0};
    uint32_t current_checkpoint{0};
    uint32_t next_checkpoint_idx{0};
    float finish_time_seconds{0.f};
};

struct RankInfo {
    size_t player_id;
    uint32_t current_checkpoint;
    float distance_to_next;
    float finish_time_seconds{0.f};
};

enum class RaceState {
    Running,
    Finished
};

struct PlayerRaceResult {
    size_t player_id;
    float finish_time_seconds;
    uint8_t position;
};

struct ParticipantResultEntry {
    uint32_t player_id;
    ParticipantState state;
    float finish_time_seconds;
    uint32_t position = 0;
};

struct RaceResult {
    std::vector<PlayerRaceResult> result;
};

#endif
