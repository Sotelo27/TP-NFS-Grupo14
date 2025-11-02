#include "race.h"
#include "race_participant.h"
#include <cmath>
#include <algorithm>

Race::Race(uint32_t id)
    : id(id) {}

void Race::add_player(size_t playerId, const CarModel& spec, float spawnX_px, float spawnY_px) {
    // Conversión simple de píxeles a "unidades" del PhysicsWorld (metros)
    // Por ahora 1 unidad = 1 metro y asumimos 32px = 1m si el cliente usa PPM=32.
    const float PPM = 32.f;
    int16_t spawnX_units = (int16_t)(spawnX_px / PPM);
    int16_t spawnY_units = (int16_t)(spawnY_px / PPM);
    parts[playerId] = RaceParticipant{ParticipantState::Active, &spec, 100.f};
    physics.create_body_with_spec(playerId, spawnX_units, spawnY_units, spec);
}

void Race::remove_player(size_t playerId) {
    auto it = parts.find(playerId);
    if (it != parts.end()) {
        it->second.state = ParticipantState::Disconnected;
    }
    physics.destroy_body(playerId);
}

void Race::update(uint32_t dtMs) {
    const float dt = (float)(dtMs) / 1000.0f;
    physics.step(dt);
}

void Race::apply_input(size_t playerId, const InputState& input) {
    // Buscar al participante correspondiente
    auto participant_it = parts.find(playerId);
    if (participant_it == parts.end()) return;

    RaceParticipant& participant = participant_it->second;
    if (participant.state != ParticipantState::Active) return;
    if (!participant.spec) return;

    const CarModel& car = *participant.spec;

    // Obtener la dirección de aceleracion y giro
    const float throttle = resolve_acceleration_input(input);
    const float steer = resolve_rotation_input(input);

    // Aplicar fuerza de aceleracion
    apply_acceleration_force(playerId, throttle, car);

    // Aplicar torque de giro solo si hay aceleracion
    if (std::fabs(throttle) > 0.01f) {
        float speed = physics.get_linear_speed(playerId);
        float k = (speed - 0.5f) / 5.0f;
        k = std::clamp(k, 0.2f, 1.0f);
        physics.apply_torque(playerId, steer * car.torqueGiro * k);
    }

    // 4) Limitar velocidad máxima
    if (car.velocidadMaxMps > 0.f) {
        physics.cap_linear_speed(playerId, car.velocidadMaxMps);
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
        player_positions.push_back(PlayerPos{(uint32_t)(playerId),pose.x,pose.y,pose.angle});
    }

    return player_positions;
}
