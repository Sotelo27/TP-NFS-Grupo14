#include "race.h"
#include "npc.h"
#include "../../common/constants.h"
#include "race_info.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../physics/building_entity.h" // Para detectar edificios

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

void Race::init_npc_spawns(const City& city) {
    npc_spawns = city.get_npc_spawns();
    for (size_t i = 0; i < npc_spawns.size(); ++i) {
        const auto& sp = npc_spawns[i];
        // Convertir a metros
        float x_m = sp.x_px / PPM;
        float y_m = sp.y_px / PPM;
        add_npc((uint8_t)(i + 1), x_m, y_m);
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
    update_npcs(dt);
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
        uint8_t hp = 0;
        auto itc = cars.find(playerId);
        Car* car_ptr = (itc != cars.end()) ? itc->second.get() : nullptr;
        if (car_ptr) {
            hp = (uint8_t)car_ptr->get_vida();
        }

        PlayerTickInfo player;
        player.username = "";
        player.car_id = participant.car_id;
        player.player_id = (uint32_t)(playerId);
        player.x = car_x_px;
        player.y = car_y_px;
        player.angle = body->GetAngle() * 180.0f / PI;
        player.health = hp;
        player.speed_mps = (car_ptr ? car_ptr->speed_mps() : 0.f) * MS_TO_KMH;

        if (car_ptr) {
            const CarModel& model = car_ptr->get_spec();
            player.max_health = (uint8_t)std::lround(model.life);
            player.improvements = model.improvements;
        } else {
            player.max_health = hp; // fallback al valor actual si no hay modelo
            player.improvements.clear();
        }

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
        // cantidad de checkpoints restantes
        player.checkpoints_remaining = (uint16_t)(track.checkpoints.size() > next_idx ? (track.checkpoints.size() - next_idx) : 0);
        
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
    // Elegir un modelo de auto para el NPC (puede ser aleatorio)
    static std::vector<CarModel> modelos = {
        car_factory::common_green_car(),
        car_factory::red_car(),
        car_factory::red_sport_car(),
        car_factory::special_car(),
        car_factory::four_by_four_convertible(),
        car_factory::pickup_truck(),
        car_factory::limousine()
    };
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist(0, modelos.size() - 1);
    CarModel modelo = modelos[dist(rng)];
    npc_models[npc_id] = modelo;

    // Crear cuerpo físico y Car para el NPC
    physics.create_car_body(10000 + npc_id, x_m, y_m, modelo); // id único para NPCs
    b2Body* body = physics.get_body(10000 + npc_id);
    npc_cars[npc_id] = std::make_unique<Car>(10000 + npc_id, modelo, body);

    // Guardar NPC para compatibilidad (puedes eliminar npcs[npc_id] si no lo usas)
    npcs[npc_id] = Npc{npc_id, x_m, y_m, 0.f, 0.f};
}

void Race::update_npcs(float dt) {
    // Parámetros IA para movimiento natural y lento, similar a usuario
    const float SENSOR_DIST = 1.2f;      // metros delante del auto
    const float SENSOR_WIDTH = 1.0f;     // ancho del sensor para evitar colisiones
    const float TARGET_SPEED_KMH = 7.0f; // velocidad objetivo baja (~2 m/s)
    const float MAX_THROTTLE = 0.10f;    // throttle máximo para NPCs (muy bajo)
    const float STEER_TO_CP = 0.18f;     // giro suave hacia checkpoint
    const float OBSTACLE_STEER = 0.35f;  // giro para evitar obstáculo
    const float OBSTACLE_THROTTLE = 0.0f;// frenar si hay obstáculo
    const float STUCK_SPEED_MPS = 0.05f; // umbral para considerar "quieto"
    const float STUCK_TIME = 1.5f;       // tiempo mínimo quieto para desbloquear

    static std::unordered_map<uint8_t, float> stuck_time_map;

    for (auto& [npc_id, car_ptr] : npc_cars) {
        if (!car_ptr) continue;
        b2Body* body = car_ptr->get_body();
        if (!body) continue;

        float angle = body->GetAngle();
        b2Vec2 pos = body->GetPosition();
        b2Vec2 forward(std::cos(angle), std::sin(angle));
        b2Vec2 sensor_center = pos + SENSOR_DIST * forward;

        // Buscar el checkpoint más cercano (camino virtual)
        float target_x = pos.x, target_y = pos.y;
        if (!track.checkpoints.empty()) {
            float min_dist = 1e9f;
            for (const auto& cp : track.checkpoints) {
                float a = cp.rotation_deg * PI / 180.0f;
                float halfW = cp.w_px * 0.5f / PPM;
                float halfH = cp.h_px * 0.5f / PPM;
                float cx = (cp.x_px + std::cos(a) * halfW - std::sin(a) * halfH) / PPM;
                float cy = (cp.y_px + std::sin(a) * halfW + std::cos(a) * halfH) / PPM;
                float dx = cx - pos.x;
                float dy = cy - pos.y;
                float d = std::sqrt(dx*dx + dy*dy);
                if (d < min_dist) {
                    min_dist = d;
                    target_x = cx;
                    target_y = cy;
                }
            }
        }

        // Calcular ángulo hacia el objetivo (checkpoint)
        float dx = target_x - pos.x;
        float dy = target_y - pos.y;
        float target_angle = std::atan2(dy, dx);
        float angle_diff = std::atan2(std::sin(target_angle - angle), std::cos(target_angle - angle));

        // Revisar colisiones con edificios usando el PhysicsWorld
        bool obstacle_ahead = false;
        float steer = 0.0f;
        PhysicsWorld& world = physics;
        for (const auto& ent_ptr : world.static_entities) {
            auto* building = dynamic_cast<BuildingEntity*>(ent_ptr.get());
            if (!building) continue;
            b2Body* b = building->get_body();
            if (!b) continue;
            b2Vec2 bpos = b->GetPosition();
            float dist = (sensor_center - bpos).Length();
            if (dist < SENSOR_WIDTH) {
                obstacle_ahead = true;
                // Determinar si el obstáculo está a la izquierda o derecha
                b2Vec2 to_obstacle = bpos - pos;
                float rel_angle = std::atan2(to_obstacle.y, to_obstacle.x) - angle;
                steer = (rel_angle > 0) ? -OBSTACLE_STEER : OBSTACLE_STEER;
                break;
            }
        }

        // Control de velocidad objetivo (mantener velocidad baja y constante)
        float speed_mps = car_ptr->speed_mps();
        float speed_kmh = speed_mps * MS_TO_KMH;
        float throttle = 0.0f;

        if (obstacle_ahead) {
            throttle = OBSTACLE_THROTTLE; // frenar si hay obstáculo
            // steer ya fue seteado arriba
        } else {
            // Mantener velocidad baja y constante
            if (speed_kmh < TARGET_SPEED_KMH - 0.5f) {
                throttle = MAX_THROTTLE;
            } else if (speed_kmh > TARGET_SPEED_KMH + 0.5f) {
                throttle = -MAX_THROTTLE; // frenar si va muy rápido
            } else {
                throttle = 0.0f; // mantener velocidad
            }
            steer = std::clamp(angle_diff, -STEER_TO_CP, STEER_TO_CP);
        }

        // Manejo de "stuck": si el auto está quieto por mucho tiempo, aplicar impulso aleatorio
        if (std::abs(speed_mps) < STUCK_SPEED_MPS && std::abs(throttle) < 0.01f) {
            stuck_time_map[npc_id] += dt;
            if (stuck_time_map[npc_id] > STUCK_TIME) {
                static std::mt19937 rng{std::random_device{}()};
                static std::uniform_real_distribution<float> random_steer(-0.12f, 0.12f);
                steer += random_steer(rng);
                throttle = 0.08f;
                stuck_time_map[npc_id] = 0.0f;
            }
        } else {
            stuck_time_map[npc_id] = 0.0f;
        }

        // Aplicar input igual que un usuario
        car_ptr->apply_input(throttle, steer);
    }
}

std::vector<NpcTickInfo> Race::snapshot_npcs() const {
    std::vector<NpcTickInfo> out;
    for (const auto& [npc_id, car_ptr] : npc_cars) {
        if (!car_ptr) continue;
        b2Body* body = car_ptr->get_body();
        if (!body) continue;
        b2Vec2 pos = body->GetPosition();
        NpcTickInfo info;
        info.npc_id = npc_id;
        info.x = static_cast<int32_t>(pos.x * PPM);
        info.y = static_cast<int32_t>(pos.y * PPM);
        out.push_back(info);
    }
    return out;
}
