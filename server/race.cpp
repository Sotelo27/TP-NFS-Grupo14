#include "race.h"
#include "race_participant.h"
#include <cmath>
#include <algorithm>

Race::Race(uint32_t id, PhysicsWorld& external_world)
    : id(id), physics(external_world) {}

void Race::add_player(size_t playerId, const CarModel& spec, float spawnX_px, float spawnY_px) {
    // Conversión de píxeles -> metros (PPM=32) -> unidades discretas (128 unidades/m)
    constexpr float PPM = 32.f;
    constexpr float UNITS_PER_METER = 128.f;
    const float spawnX_m = spawnX_px / PPM;
    const float spawnY_m = spawnY_px / PPM;
    const int16_t spawnX_units = (int16_t)(std::lround(spawnX_m * UNITS_PER_METER));
    const int16_t spawnY_units = (int16_t)(std::lround(spawnY_m * UNITS_PER_METER));
    parts[playerId] = RaceParticipant{ParticipantState::Active, &spec};
    physics.create_car_body(playerId, spawnX_units, spawnY_units, spec);
    cars[playerId] = std::make_unique<Car>(playerId, spec, physics.get_body(playerId));
}

void Race::remove_player(size_t playerId) {
    auto it = parts.find(playerId);
    if (it != parts.end()) {
        it->second.state = ParticipantState::Disconnected;
    }
    // Remover entidad Car asociada
    auto itc = cars.find(playerId);
    if (itc != cars.end()) {
        cars.erase(itc);
    }
    physics.destroy_body(playerId);
}

void Race::apply_input(size_t playerId, const InputState& input) {
    // Buscar al participante correspondiente
    auto participant_it = parts.find(playerId);
    if (participant_it == parts.end()) return;

    RaceParticipant& participant = participant_it->second;
    if (participant.state != ParticipantState::Active) return;
    if (!participant.spec) return;

    // Resolver entradas en [-1..1]
    const float throttle = resolve_acceleration_input(input);
    const float steer = resolve_rotation_input(input);

    // Delegar aplicacion de input a Car,asi desacoplamos race
    auto itc = cars.find(playerId);
    if (itc != cars.end() && itc->second) {
        itc->second->apply_input(throttle, steer);
    }
}

float Race::resolve_acceleration_input(const InputState& input) {
    if (input.up) {
        return 1.f;
    }
    if (input.down) {
        return -1.f;
    }
    return 0.f;
}

float Race::resolve_rotation_input(const InputState& input) {
    return (input.right ? 1.f : 0.f) - (input.left ? 1.f : 0.f);
}

void Race::apply_acceleration_force(size_t player_id, float throttle, const CarModel& car) {
    // Fuerza longitudinal en el eje forward del auto (usa ángulo actual)
    float ang = physics.get_angle(player_id);
    float fx = throttle * car.fuerzaAceleracionN * std::cos(ang);
    float fy = throttle * car.fuerzaAceleracionN * std::sin(ang);
    physics.apply_force_center(player_id, fx, fy);
}


std::vector<PlayerPos> Race::snapshot_poses() const {
    std::vector<PlayerPos> player_positions;
    player_positions.reserve(parts.size());

    for (const auto& [playerId, participant] : parts) {
        if (participant.state != ParticipantState::Active && participant.state != ParticipantState::Finished) {
            continue;
        }
        Pose pose = physics.get_pose(playerId);

        const double PIXELS_PER_UNIT = 0.25;
        const int16_t x_px = (int16_t)(std::lround((double)(pose.x) * PIXELS_PER_UNIT));
        const int16_t y_px = (int16_t)(std::lround((double)(pose.y) * PIXELS_PER_UNIT));
        player_positions.push_back(PlayerPos{(uint32_t)(playerId), x_px, y_px, pose.angle});
    }

    return player_positions;
}
