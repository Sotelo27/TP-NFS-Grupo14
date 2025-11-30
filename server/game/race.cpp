#include "race.h"
#include "npc.h"
#include "../../common/constants.h"
#include "race_info.h"
#include <cmath>
#include <algorithm>
#include <iostream>

#define  PI 3.14159265358979323846f
#define PPM 32.f
#define MAX_DURATION_SECONDS 600.f
#define MIN_DURATION_SECONDS 0.f
#define MS_TO_KMH 3.6f

Race::Race(uint32_t id, PhysicsWorld& external_world)
    : id(id), physics(external_world), track() {}

void Race::add_player(size_t playerId, const CarModel& spec, uint8_t car_id, float spawnX_px, float spawnY_px) {
    // Conversión directa de píxeles a metros (PPM=32)
    const float spawnX_m = spawnX_px / PPM;
    const float spawnY_m = spawnY_px / PPM;
    parts[playerId] = RaceParticipant{ParticipantState::Active, car_id};
    physics.create_car_body(playerId, spawnX_m, spawnY_m, spec);
    cars[playerId] = std::make_unique<Car>(playerId, spec, physics.get_body(playerId));
    cars_by_player[playerId] = cars[playerId].get();
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
void Race::on_car_checkpoint(const std::string& race_id, size_t player_id, uint32_t checkpoint_id) {
    // Log básico del evento recibido
    std::cout << "[Race] CP event: route='" << race_id
              << "' player_id=" << player_id
              << " checkpoint_id=" << checkpoint_id
              << " (track_route='" << track.route_id << "')\n";

    // si no es mi recorrido ignoro
    if (race_id != track.route_id) {
        std::cout << "[Race] Ignored: route mismatch (event='" << race_id
                  << "' vs track='" << track.route_id << "')\n";
        return;
    }

    auto it = parts.find(player_id);
    if (it == parts.end()) return;

    RaceParticipant& p = it->second;
    if (p.state != ParticipantState::Active) return;

    if (checkpoint_id == p.next_checkpoint_idx) {
        p.current_checkpoint = checkpoint_id;
        ++p.next_checkpoint_idx;

        if (p.next_checkpoint_idx == track.checkpoint_count) {
            p.state = ParticipantState::Finished;
            // Registrar tiempo de llegada del jugador
            p.finish_time_seconds = race_duration;
            std::cout << "[Race] Player " << player_id << " FINISHED route='"
                      << track.route_id << "' total_cp=" << track.checkpoint_count << "\n";
        } else {
            std::cout << "[Race] Player " << player_id << " advanced to checkpoint "
                      << p.current_checkpoint << " (next=" << p.next_checkpoint_idx
                      << "/" << track.checkpoint_count << ")\n";
        }
    } else {
        std::cout << "[Race] Ignored: wrong order for player " << player_id
                  << " (got=" << checkpoint_id << ", expected=" << p.next_checkpoint_idx
                  << ")\n";
    }
}

void Race::set_track(const Track& new_track) {
    track = new_track;
    std::cout << "[Race] Track set: route='" << track.route_id
              << "' checkpoints=" << track.checkpoint_count << "\n";
}

bool Race::is_finished() const noexcept {
    return state_ == RaceState::Finished;
}

const std::string& Race::get_route_id() const {
    return track.route_id;
}

void Race::advance_time(float dt) {
    race_duration += dt;
    update_npcs(dt); // Actualiza NPCs en cada tick
    check_health_states();
    check_time_limit();
    evaluate_finish();
}

void Race::check_health_states() {
    for (auto& [player_id, participant] : parts) {
        if (participant.state != ParticipantState::Active) {
            continue;
        }

        auto it_car = cars.find(player_id);
        if (it_car == cars.end() || !it_car->second) {
            continue;
        }

        if (it_car->second->get_vida() <= 0.f) {
            participant.state = ParticipantState::Disqualified;
            participant.finish_time_seconds = race_duration;

            std::cout << "[Race] Player " << player_id
                      << " DISQUALIFIED (no health)\n";
        }
    }
}

void Race::check_time_limit() {
    if (state_ == RaceState::Finished || race_duration < MAX_DURATION_SECONDS) {
        return;
    }

    std::cout << "[TIME FINISH] Race duration exceeded maximum allowed time. Ending race.\n";

    for (auto& [player_id, participant] : parts) {
        if (participant.state == ParticipantState::Active) {
            participant.state = ParticipantState::Disqualified;
            participant.finish_time_seconds = race_duration;
        }
    }

    state_ = RaceState::Finished;
}

void Race::evaluate_finish() {
    if (state_ == RaceState::Finished) return;

    bool any_active = false;
    for (const auto& [pid, participant] : parts) {
        if (participant.state == ParticipantState::Active) {
            any_active = true;
            break;
        }
    }

    if (!any_active) {
        state_ = RaceState::Finished;
        std::cout << "[Race] All participants finished/disqualified/disconnected. Race finished." << std::endl;
    }
}

uint32_t Race::get_race_time_seconds() const {
    float remaining = MAX_DURATION_SECONDS - race_duration;
    if (remaining < MIN_DURATION_SECONDS) remaining = MIN_DURATION_SECONDS;
    return (uint32_t)(remaining);
}

RaceResult Race::build_race_results() const {
    RaceResult r;
    r.result.reserve(parts.size());
    for (const auto& [id, participant] : parts) {
        r.result.push_back(ParticipantResultEntry{
            (uint32_t)(id),
            participant.state,
            participant.finish_time_seconds
        });
    }

    std::sort(r.result.begin(), r.result.end(),
              [](const ParticipantResultEntry& a,
                 const ParticipantResultEntry& b) {
                  return a.finish_time_seconds < b.finish_time_seconds;
              });

    uint32_t pos = 1;
    for (auto& entry : r.result) {
        entry.position = pos++;
    }
    return r;
}

void Race::clear_cars() {
    std::cout << "[Race] Clearing cars for race id=" << id << " count=" << cars.size() << "\n";
    // Destruir cuerpos en el mundo físico
    for (const auto& kv : cars) {
        size_t pid = kv.first;
        physics.destroy_body(pid);
    }
    cars.clear();
    parts.clear();
    std::cout << "[Race] Clear complete for race id=" << id << "\n";
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

bool Race::compare_rank(const RankInfo& a, const RankInfo& b) {
    if (a.checkpoints_done != b.checkpoints_done)
        // mientras mas checkpoints - mejor tendra su posicion
        return a.checkpoints_done > b.checkpoints_done;
    // Si empatan en checkpoints, gana el que este mas cerca al siguiente checkpoint
    return a.distance_to_next_px < b.distance_to_next_px;
}

void Race::calculate_ranking_positions(std::vector<PlayerTickInfo>& player_tick_info,
                                 std::vector<RankInfo>& ranking) const {

    std::sort(ranking.begin(), ranking.end(),compare_rank);

    for (std::size_t i = 0; i < ranking.size(); ++i) {
        const uint32_t pid = ranking[i].player_id;
        const uint16_t pos = (uint16_t)(i + 1);

        for (auto& player : player_tick_info) {
            if (player.player_id == pid) {
                player.position_in_race = pos;
                break;
            }
        }
    }
}

std::vector<PlayerTickInfo> Race::snapshot_ticks() const {
    std::vector<PlayerTickInfo> out;
    out.reserve(parts.size());

    std::vector<RankInfo> ranking;
    ranking.reserve(parts.size());

    for (const auto& [playerId, participant] : parts) {
        if (participant.state != ParticipantState::Active && participant.state != ParticipantState::Finished && participant.state != ParticipantState::Disqualified) {
            continue;
        }

        b2Body* body = physics.get_body(playerId);
        if (!body) continue;
        b2Vec2 p = body->GetPosition();
        const int32_t car_x_px = (int32_t)std::lround(p.x * PPM);
        const int32_t car_y_px = (int32_t)std::lround(p.y * PPM);
        uint8_t hp = 100;
        auto itc = cars.find(playerId);

        if (itc != cars.end() && itc->second) {
            float vida = itc->second->get_vida();
            hp = (uint8_t)(vida);
        }

        PlayerTickInfo player;
        player.username = "";
        player.car_id = participant.car_id;
        player.player_id = (uint32_t)(playerId);
        player.x = car_x_px;
        player.y = car_y_px;
        player.angle = body->GetAngle() * 180.0f / PI;
        player.health = hp;
        player.speed_mps = ((itc != cars.end() && itc->second) ? itc->second->speed_mps() : 0.f ) * MS_TO_KMH;

        // Siguiente checkpoint
        player.x_checkpoint = 0;
        player.y_checkpoint = 0;
        player.hint_angle_deg = 0.0f;
        player.position_in_race = 0;
        player.distance_to_checkpoint = 0.0f;
        
        const uint32_t next_idx = participant.next_checkpoint_idx;
        float distance_px = 0.0f;
        if (!track.checkpoints.empty() && next_idx < track.checkpoints.size()) {
            const auto& cp = track.checkpoints[next_idx];
            
            const float a = cp.rotation_deg * PI / 180.0f;
            const float halfW = cp.w_px * 0.5f;
            const float halfH = cp.h_px * 0.5f;
                                   //[Oringen] + [rotacion]
            const float checkp_center_x_px = cp.x_px + std::cos(a) * halfW - std::sin(a) * halfH;
            const float checkp_center_y_px = cp.y_px + std::sin(a) * halfW + std::cos(a) * halfH;

            player.x_checkpoint = (uint16_t)(std::lround(checkp_center_x_px));
            player.y_checkpoint = (uint16_t)(std::lround(checkp_center_y_px));
            // Distancia en píxeles entre auto y checkpoint
            const float dx_px = checkp_center_x_px - car_x_px;
            const float dy_px = checkp_center_y_px - car_y_px;
            distance_px = std::sqrt(dx_px * dx_px + dy_px * dy_px);
            player.distance_to_checkpoint = distance_px;

            // Angulo del hint hacia el checkpoint
            const float angle_rad = std::atan2(dy_px, dx_px);
            player.hint_angle_deg = angle_rad * 180.0f / PI;
        } else{
            // No hay mas checkpoints, dejamos los valores por defecto
            player.x_checkpoint = 0;
            player.y_checkpoint = 0;
            player.hint_angle_deg = 0.0f;
        }
        
        ranking.push_back(RankInfo{(uint32_t)(playerId), participant.next_checkpoint_idx, distance_px, participant.finish_time_seconds});
        out.push_back(player);
    }

    calculate_ranking_positions(out, ranking);

    return out;
}

void Race::apply_cheat(size_t playerId, uint8_t cheat_code) {
    auto it = cars.find(playerId);
    if (it == cars.end()) return;
    if (cheat_code == CHEAT_INFINITE_LIFE) {
        it->second->set_infinite_life(true);
        it->second->set_vida(it->second->get_spec().life); // Restaura vida máxima
        std::cout << "[Race] Cheat de vida infinita activado para playerId=" << playerId << "\n";
    } else if (cheat_code == CHEAT_TELEPORT_NEXT_CHECKPOINT) {
        teleport_to_next_checkpoint(playerId);
    } else if (cheat_code == CHEAT_WIN_RACE) {
        cheat_win_race(playerId);
    }
}

void Race::teleport_to_next_checkpoint(size_t playerId) {
    auto it_part = parts.find(playerId);
    if (it_part == parts.end()) return;
    auto it_car = cars.find(playerId);
    if (it_car == cars.end() || !it_car->second) return;
    if (track.checkpoints.empty()) return;

    RaceParticipant& participant = it_part->second;
    uint32_t next_idx = participant.next_checkpoint_idx;
    if (next_idx >= track.checkpoints.size()) {
        std::cout << "[Race] Teleport: No hay más checkpoints para playerId=" << playerId << "\n";
        return;
    }
    const auto& cp = track.checkpoints[next_idx];

    // Calcular el centro del checkpoint
    const float a = cp.rotation_deg * PI / 180.0f;
    const float halfW = cp.w_px * 0.5f;
    const float halfH = cp.h_px * 0.5f;
    const float checkp_center_x_px = cp.x_px + std::cos(a) * halfW - std::sin(a) * halfH;
    const float checkp_center_y_px = cp.y_px + std::sin(a) * halfW + std::cos(a) * halfH;

    // Convertir a metros
    const float x_m = checkp_center_x_px / PPM;
    const float y_m = checkp_center_y_px / PPM;

    b2Body* body = it_car->second->get_body();
    if (!body) return;

    // Teletransportar el auto
    body->SetTransform(b2Vec2(x_m, y_m), a);
    body->SetLinearVelocity(b2Vec2(0, 0));
    body->SetAngularVelocity(0);

    std::cout << "[Race] Cheat de teletransporte: playerId=" << playerId
              << " movido a checkpoint " << next_idx << " (" << x_m << ", " << y_m << ")\n";
}

void Race::cheat_win_race(size_t playerId) {
    auto it_part = parts.find(playerId);
    if (it_part == parts.end()) return;
    RaceParticipant& participant = it_part->second;
    if (participant.state != ParticipantState::Active) return;
    if (track.checkpoints.empty()) return;

    // Marca todos los checkpoints como recorridos
    participant.current_checkpoint = (uint32_t)(track.checkpoints.size() - 1);
    participant.next_checkpoint_idx = (uint32_t)(track.checkpoints.size());
    participant.state = ParticipantState::Finished;
    participant.finish_time_seconds = race_duration;

    std::cout << "[Race] Cheat WIN_RACE: playerId=" << playerId << " ha ganado la carrera automáticamente.\n";
}

void Race::add_npc(uint8_t npc_id, float x_m, float y_m) {
    npcs[npc_id] = Npc{npc_id, x_m, y_m, 0.f, 0.f};
}

void Race::update_npcs(float dt) {
    for (auto& [id, npc] : npcs) {
        // Ejemplo: movimiento simple en línea recta
        npc.x_m += npc.vx * dt;
        npc.y_m += npc.vy * dt;
        // Se puede agregar logica de IA (ojito peluche)
    }
}

std::vector<NpcTickInfo> Race::snapshot_npcs() const {
    std::vector<NpcTickInfo> out;
    for (const auto& [id, npc] : npcs) {
        NpcTickInfo info;
        info.npc_id = npc.npc_id;
        info.x = static_cast<int32_t>(npc.x_m * PPM);
        info.y = static_cast<int32_t>(npc.y_m * PPM);
        out.push_back(info);
    }
    return out;
}
