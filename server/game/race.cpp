#include "race.h"
#include "race_participant.h"
#include <cmath>
#include <algorithm>
#include <iostream>

#define  PI 3.14159265358979323846f
#define PPM 32.f
#define MAX_DURATION_SECONDS 600.f

Race::Race(uint32_t id, PhysicsWorld& external_world)
    : id(id), physics(external_world), track() {}

void Race::add_player(size_t playerId, const CarModel& spec, uint8_t car_id, float spawnX_px, float spawnY_px) {
    // Conversión directa de píxeles a metros (PPM=32)
    const float spawnX_m = spawnX_px / PPM;
    const float spawnY_m = spawnY_px / PPM;
    parts[playerId] = RaceParticipant{ParticipantState::Active, car_id};
    physics.create_car_body(playerId, spawnX_m, spawnY_m, spec);
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

    // Resolver entradas en [-1..1]
    const float throttle = resolve_acceleration_input(input);
    const float steer = resolve_rotation_input(input);

    // Delegar aplicacion de input a Car,asi desacoplamos race
    auto itc = cars.find(playerId);
    if (itc != cars.end() && itc->second) {
        itc->second->apply_input(throttle, steer);
    }
}
void Race::on_car_checkpoint(const std::string& race_id,
                             size_t player_id,
                             uint32_t checkpoint_id) {
    // 1) si no es mi recorrido, ignoro
    if (race_id != track.route_id) return;

    auto it = parts.find(player_id);
    if (it == parts.end()) return;

    RaceParticipant& p = it->second;
    if (p.state != ParticipantState::Active) return;

    // 2) validación de orden
    if (checkpoint_id == p.current_checkpoint + 1) {
        p.current_checkpoint = checkpoint_id;

        if (checkpoint_id + 1 == track.checkpoint_count) {
            p.state = ParticipantState::Finished;
            p.finished = true;
        }
    }
}

void Race::set_track(const Track& new_track) {
    track = new_track;
}

bool Race::is_finished() const noexcept {
    return is_finished_;
}

void Race::advance_time(float dt) {
    race_duration += dt;

    if (!is_finished_ && race_duration >= MAX_DURATION_SECONDS) {
        std::cout << "[TIME FINISH]Race duration exceeded maximum allowed time. Ending race." << std::endl;

        for (auto& [playerId, participant] : parts) {
            if (participant.state == ParticipantState::Active) {
                participant.state = ParticipantState::Disqualified;
            }
        }

        is_finished_ = true;
    }
}


uint32_t Race::get_race_time_seconds() const {
    return (uint32_t)(race_duration);
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

std::vector<PlayerPos> Race::snapshot_poses() const {
    std::vector<PlayerPos> player_positions;
    player_positions.reserve(parts.size());

    for (const auto& [playerId, participant] : parts) {
        if (participant.state != ParticipantState::Active && participant.state != ParticipantState::Finished) {
            continue;
        }
        b2Body* body = physics.get_body(playerId);
        if (!body) continue;
        b2Vec2 p = body->GetPosition();
        const int16_t x_px = (int16_t)std::lround(p.x * PPM);
        const int16_t y_px = (int16_t)std::lround(p.y * PPM);
        player_positions.push_back(PlayerPos{(uint32_t)(playerId), x_px, y_px, body->GetAngle()});
    }

    return player_positions;
}

std::vector<PlayerTickInfo> Race::snapshot_ticks() const {
    std::vector<PlayerTickInfo> out;
    out.reserve(parts.size());

    for (const auto& [playerId, participant] : parts) {
        if (participant.state != ParticipantState::Active && participant.state != ParticipantState::Finished) {
            continue;
        }

        b2Body* body = physics.get_body(playerId);
        if (!body) continue;
        b2Vec2 p = body->GetPosition();
        const int32_t x_px = (int32_t)std::lround(p.x * PPM);
        const int32_t y_px = (int32_t)std::lround(p.y * PPM);
        uint8_t hp = 100;
        auto itc = cars.find(playerId);

        if (itc != cars.end() && itc->second) {
            float vida = itc->second->get_vida();
            if (vida < 0.f) vida = 0.f;
            if (vida > 100.f) vida = 100.f;
            hp = (uint8_t)std::lround(vida);
        }

        PlayerTickInfo pti;
        pti.username = "";
        pti.car_id = participant.car_id;
        pti.player_id = (uint32_t)(playerId);
        pti.x = x_px;
        pti.y = y_px;
        pti.angle = body->GetAngle() * 180.0f / PI;
        pti.health = hp;
        pti.speed_mps = (itc != cars.end() && itc->second) ? itc->second->speed_mps() : 0.f;
        out.push_back(pti);
    }

    return out;
}
