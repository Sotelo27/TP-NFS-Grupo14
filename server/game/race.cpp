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
    available_routes = city.get_routes();
    
    std::cout << "[Race] Initializing NPCs:\n";
    std::cout << "  - " << npc_spawns.size() << " NPC spawn points\n";
    std::cout << "  - " << available_routes.size() << " routes available\n";
    
    if (available_routes.empty()) {
        std::cout << "[Race] WARNING: No routes available! NPCs will move forward only.\n";
    }
    
    for (size_t i = 0; i < npc_spawns.size(); ++i) {
        const auto& sp = npc_spawns[i];
        float x_m = sp.x_px / PPM;
        float y_m = sp.y_px / PPM;
        uint8_t npc_id = (uint8_t)(i + 1);
        add_npc(npc_id, x_m, y_m);
        
        if (!available_routes.empty()) {
            init_npc_navigation(npc_id, x_m, y_m);
        }
    }
}

void Race::init_npc_navigation(uint8_t npc_id, float x_m, float y_m) {
    if (available_routes.empty()) {
        std::cout << "[Race] ERROR: No routes available for NPC " << (int)npc_id << "\n";
        return;
    }

    float x_px = x_m * PPM;
    float y_px = y_m * PPM;
    
    std::cout << "[Race] Initializing navigation for NPC " << (int)npc_id 
              << " at (" << x_px << ", " << y_px << ") px\n";
    
    // Buscar EL WAYPOINT MÁS CERCANO de TODAS las rutas
    const Route* best_route = nullptr;
    uint32_t best_waypoint_index = 0;
    float min_distance = 1e9f;
    
    for (const auto& route : available_routes) {
        if (route.waypoints.empty()) continue;
        
        for (size_t i = 0; i < route.waypoints.size(); ++i) {
            const auto& wp = route.waypoints[i];
            float dx = wp.x_px - x_px;
            float dy = wp.y_px - y_px;
            float dist = std::sqrt(dx*dx + dy*dy);
            
            if (dist < min_distance) {
                min_distance = dist;
                best_route = &route;
                best_waypoint_index = (uint32_t)i;
            }
        }
    }
    
    if (best_route) {
        npc_nav_states[npc_id] = NpcNavigationState{
            best_route->route_id,
            best_waypoint_index,
            false
        };
        
        std::cout << "[Race] ✓ NPC " << (int)npc_id << " ASSIGNED to route '" 
                  << best_route->route_id << "' starting at waypoint " 
                  << best_waypoint_index << "/" << best_route->waypoints.size() 
                  << " (distance: " << min_distance << " px)\n";
    } else {
        std::cout << "[Race] ERROR: Could not assign route to NPC " << (int)npc_id << "\n";
    }
}

void Race::update_npcs(float dt) {
    const float TARGET_SPEED_KMH = 28.0f;
    const float BASE_THROTTLE = 0.35f;
    const float MAX_THROTTLE = 0.7f;
    const float MAX_STEER = 0.32f;
    const float WAYPOINT_REACH_DISTANCE = 16.0f;
    const float OBSTACLE_AVOID_DISTANCE = 7.5f;
    const float NPC_SEPARATION_RADIUS = 5.5f;       // evitar chocarse entre NPCs
    const float NPC_SEPARATION_STEER_GAIN = 0.8f;   // cuánto corrige el steering por separación
    const float STUCK_THRESHOLD = 0.08f;
    const float STUCK_TIME_LIMIT = 1.1f;

    static std::unordered_map<uint8_t, float> stuck_time_map;

    auto align_body = [](b2Body* body, float x_m, float y_m, float target_angle) {
        body->SetTransform(b2Vec2(x_m, y_m), target_angle);
        body->SetLinearVelocity(b2Vec2(0.f, 0.f));
        body->SetAngularVelocity(0.f);
        body->SetAwake(true);
    };

    for (auto& [npc_id, car_ptr] : npc_cars) {
        if (!car_ptr) continue;
        b2Body* body = car_ptr->get_body();
        if (!body) continue;

        // Forzar awake cada frame
        body->SetSleepingAllowed(false);
        body->SetAwake(true);

        float throttle = BASE_THROTTLE;
        float steer = 0.0f;

        // SI NO HAY RUTAS: SOLO IR HACIA ADELANTE
        if (available_routes.empty()) {
            // Detección de obstáculos
            bool obstacle_ahead = false;
            b2Vec2 pos = body->GetPosition();
            float angle = body->GetAngle();
            
            for (const auto& ent_ptr : physics.static_entities) {
                auto* building = dynamic_cast<BuildingEntity*>(ent_ptr.get());
                if (!building) continue;
                
                b2Body* b = building->get_body();
                if (!b) continue;

                b2Vec2 bpos = b->GetPosition();
                float dist = (pos - bpos).Length();

                if (dist < OBSTACLE_AVOID_DISTANCE) {
                    obstacle_ahead = true;
                    b2Vec2 to_obstacle = bpos - pos;
                    float obstacle_angle = std::atan2(to_obstacle.y, to_obstacle.x);
                    float relative_angle = obstacle_angle - angle;
                    steer = (relative_angle > 0) ? -0.5f : 0.5f;
                    break;
                }
            }
            
            if (obstacle_ahead) {
                throttle = -0.3f;
            } else {
                float speed_kmh = car_ptr->speed_mps() * MS_TO_KMH;
                if (speed_kmh < TARGET_SPEED_KMH) {
                    throttle = MAX_THROTTLE;
                } else {
                    throttle = MAX_THROTTLE * 0.7f;
                }
            }
            
            car_ptr->apply_input(throttle, steer);
            continue;
        }

        // NAVEGACIÓN CON RUTAS
        auto nav_it = npc_nav_states.find(npc_id);
        if (nav_it == npc_nav_states.end()) {
            b2Vec2 pos = body->GetPosition();
            init_npc_navigation(npc_id, pos.x, pos.y);
            car_ptr->apply_input(BASE_THROTTLE, 0.0f);
            continue;
        }

        NpcNavigationState& nav = nav_it->second;

        // Si completó la ruta, buscar una nueva
        if (nav.route_completed) {
            b2Vec2 pos = body->GetPosition();
            init_npc_navigation(npc_id, pos.x, pos.y);
            car_ptr->apply_input(BASE_THROTTLE, 0.0f);
            continue;
        }

        // Buscar la ruta actual
        const Route* current_route = nullptr;
        for (const auto& route : available_routes) {
            if (route.route_id == nav.current_route_id) {
                current_route = &route;
                break;
            }
        }

        if (!current_route || current_route->waypoints.empty()) {
            nav.route_completed = true;
            continue;
        }

        if (nav.current_waypoint_index >= current_route->waypoints.size()) {
            nav.route_completed = true;
            continue;
        }

        // OBTENER WAYPOINT ACTUAL
        const Waypoint& target_wp = current_route->waypoints[nav.current_waypoint_index];
        float target_x_m = target_wp.x_px / PPM;
        float target_y_m = target_wp.y_px / PPM;

        b2Vec2 pos = body->GetPosition();
        float angle = body->GetAngle();

        float dx = target_x_m - pos.x;
        float dy = target_y_m - pos.y;
        float dist_to_waypoint = std::sqrt(dx*dx + dy*dy);

        // Si llegó al waypoint, avanzar
        if (dist_to_waypoint < WAYPOINT_REACH_DISTANCE) {
            nav.current_waypoint_index++;
            if (nav.current_waypoint_index >= current_route->waypoints.size()) {
                nav.route_completed = true;
                std::cout << "[Race] NPC " << (int)npc_id << " completed route '"
                          << nav.current_route_id << "'\n";
            }
            car_ptr->apply_input(BASE_THROTTLE, 0.0f);
            continue;
        }

        // Dirección hacia el waypoint
        float target_angle = std::atan2(dy, dx);
        float angle_diff = std::atan2(std::sin(target_angle - angle), std::cos(target_angle - angle));

        // 1) Separación entre NPCs para evitar choques
        {
            b2Vec2 pos = body->GetPosition();
            float separation_steer = 0.0f;
            for (const auto& [other_id, other_car] : npc_cars) {
                if (other_id == npc_id || !other_car) continue;
                b2Body* ob = other_car->get_body();
                if (!ob) continue;
                float dist = (pos - ob->GetPosition()).Length();
                if (dist < NPC_SEPARATION_RADIUS && dist > 0.001f) {
                    b2Vec2 away = pos - ob->GetPosition();
                    float away_angle = std::atan2(away.y, away.x);
                    float rel = std::atan2(std::sin(away_angle - angle), std::cos(away_angle - angle));
                    separation_steer += std::clamp(rel * NPC_SEPARATION_STEER_GAIN, -MAX_STEER, MAX_STEER);
                }
            }
            steer += separation_steer;
        }

        // 2) Detección simple de edificios por “sensor” frontal
        bool obstacle_ahead = false;
        float obstacle_steer = 0.0f;
        {
            b2Vec2 pos = body->GetPosition();
            // punto a ~OBSTACLE_AVOID_DISTANCE metros por delante
            b2Vec2 forward(std::cos(angle), std::sin(angle));
            b2Vec2 sensor = pos + OBSTACLE_AVOID_DISTANCE * forward;

            for (const auto& ent_ptr : physics.static_entities) {
                auto* building = dynamic_cast<BuildingEntity*>(ent_ptr.get());
                if (!building) continue;
                b2Body* b = building->get_body();
                if (!b) continue;

                float dist = (sensor - b->GetPosition()).Length();
                if (dist < (OBSTACLE_AVOID_DISTANCE * 0.8f)) {
                    obstacle_ahead = true;
                    // desvío lateral según dónde está el obstáculo
                    b2Vec2 to_ob = b->GetPosition() - pos;
                    float ob_ang = std::atan2(to_ob.y, to_ob.x);
                    float rel = std::atan2(std::sin(ob_ang - angle), std::cos(ob_ang - angle));
                    obstacle_steer = (rel > 0.f) ? -MAX_STEER : MAX_STEER;
                    break;
                }
            }
        }

        float speed_mps = car_ptr->speed_mps();
        float speed_kmh = speed_mps * MS_TO_KMH;

        // 3) Control de velocidad y steering
        if (obstacle_ahead) {
            throttle = BASE_THROTTLE * 0.2f; // desacelerar pero no frenar del todo
            steer = std::clamp(steer + obstacle_steer, -MAX_STEER, MAX_STEER);
        } else {
            if (speed_kmh < TARGET_SPEED_KMH - 8.0f) {
                throttle = std::max(throttle, MAX_THROTTLE * 0.85f);
            } else if (speed_kmh > TARGET_SPEED_KMH + 8.0f) {
                throttle = BASE_THROTTLE * 0.5f;
            } else {
                throttle = std::max(throttle, MAX_THROTTLE * 0.65f);
            }
            float steer_cmd = std::clamp(angle_diff * 1.15f, -MAX_STEER, MAX_STEER);
            steer = std::clamp(steer + steer_cmd, -MAX_STEER, MAX_STEER);
        }

        // 4) Anti-stuck: reubicar y alinear hacia el siguiente waypoint si se traba
        if (std::abs(speed_mps) < STUCK_THRESHOLD) {
            stuck_time_map[npc_id] += dt;
            if (stuck_time_map[npc_id] > STUCK_TIME_LIMIT) {
                // empujar 2 metros hacia el objetivo y alinear ángulo
                const float push_m = 2.0f;
                float nx = pos.x + std::cos(target_angle) * push_m;
                float ny = pos.y + std::sin(target_angle) * push_m;
                align_body(body, nx, ny, target_angle);
                stuck_time_map[npc_id] = 0.0f;
                throttle = MAX_THROTTLE * 0.9f;
                steer = 0.0f;
                std::cout << "[Race] NPC " << (int)npc_id << " unstuck relocate\n";
                // tras reubicar, seguir al mismo waypoint sin saltarlo
            }
        } else {
            stuck_time_map[npc_id] = 0.0f;
        }

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
        info.angle = body->GetAngle() * 180.0f / PI;
        out.push_back(info);
    }
    return out;
}

void Race::add_npc(uint8_t npc_id, float x_m, float y_m) {
    CarModel npc_model;
    npc_model.masaKg = 400.0f;
    npc_model.velocidadMaxMps = 7.5f;
    npc_model.fuerzaAceleracionN = 380.0f;
    npc_model.torqueGiro = 3.0f;
    npc_model.dampingLineal = 0.6f;
    npc_model.dampingAngular = 2.2f;
    npc_model.life = 100.0f;

    size_t npc_body_id = 1000000 + npc_id;
    physics.create_car_body(npc_body_id, x_m, y_m, npc_model);
    b2Body* body = physics.get_body(npc_body_id);
    if (!body) {
        std::cout << "[Race] ERROR: Failed to create body for NPC " << (int)npc_id << "\n";
        return;
    }
    body->SetSleepingAllowed(false);
    body->SetAwake(true);
    // pequeña inclinación aleatoria para evitar superposición exacta
    // (no crítico, pero ayuda a no “pegarse” todos con el mismo heading)
    // Nota: si no desea aleatoriedad, comentar las dos líneas siguientes.
    // float jitter = (npc_id % 5) * 0.03f;
    // body->SetTransform(body->GetPosition(), body->GetAngle() + jitter);

    npc_cars[npc_id] = std::make_unique<Car>(npc_body_id, npc_model, body);
    npc_models[npc_id] = npc_model;
    
    std::cout << "[Race] Added NPC " << (int)npc_id 
              << " at (" << x_m << ", " << y_m << ")\n";
}

void Race::set_track(const Track& new_track) {
    track = new_track;
}

const std::string& Race::get_route_id() const {
    return track.route_id;
}

void Race::advance_time(float dt) {
    race_duration += dt;
    check_health_states();
    check_time_limit();
    update_npcs(dt);
    evaluate_finish();
}

uint32_t Race::get_race_time_seconds() const {
    return static_cast<uint32_t>(race_duration);
}

bool Race::is_finished() const noexcept {
    return state_ == RaceState::Finished;
}

void Race::check_health_states() {
    for (auto& kv : parts) {
        RaceParticipant& p = kv.second;
        if (p.state != ParticipantState::Active) continue;
        
        auto car_it = cars.find(kv.first);
        if (car_it != cars.end() && car_it->second) {
            if (car_it->second->get_vida() <= 0.0f) {
                p.state = ParticipantState::Crashed;
                std::cout << "[Race] Player " << kv.first << " crashed (no health)\n";
            }
        }
    }
}

void Race::check_time_limit() {
    if (race_duration >= MAX_DURATION_SECONDS) {
        for (auto& kv : parts) {
            if (kv.second.state == ParticipantState::Active) {
                kv.second.state = ParticipantState::TimeOut;
            }
        }
    }
}

void Race::evaluate_finish() {
    bool has_active = false;
    for (const auto& kv : parts) {
        if (kv.second.state == ParticipantState::Active) {
            has_active = true;
            break;
        }
    }
    if (!has_active) {
        state_ = RaceState::Finished;
    }
}

float Race::resolve_acceleration_input(const InputState& input) {
    if (input.up && !input.down) return 1.0f;
    if (input.down && !input.up) return -1.0f;
    return 0.0f;
}

float Race::resolve_rotation_input(const InputState& input) {
    if (input.right && !input.left) return 1.0f;
    if (input.left && !input.right) return -1.0f;
    return 0.0f;
}

std::vector<PlayerTickInfo> Race::snapshot_ticks() const {
    std::vector<PlayerTickInfo> ticks;
    std::vector<RankInfo> ranking;
    
    for (const auto& kv : parts) {
        const size_t pid = kv.first;
        const RaceParticipant& p = kv.second;
        
        auto car_it = cars.find(pid);
        if (car_it == cars.end() || !car_it->second) continue;
        
        Car* car = car_it->second.get();
        b2Body* body = car->get_body();
        if (!body) continue;
        
        b2Vec2 pos = body->GetPosition();
        float angle_rad = body->GetAngle();
        
        PlayerTickInfo info;
        info.player_id = static_cast<uint32_t>(pid);
        info.x = static_cast<int32_t>(pos.x * PPM);
        info.y = static_cast<int32_t>(pos.y * PPM);
        info.angle = angle_rad * 180.0f / PI;
        info.health = static_cast<uint8_t>(car->get_vida());
        info.max_health = static_cast<uint8_t>(car->get_spec().life);
        info.speed_mps = car->speed_mps();
        info.car_id = p.car_id;
        
        if (p.next_checkpoint_idx < track.checkpoints.size()) {
            const auto& cp = track.checkpoints[p.next_checkpoint_idx];
            info.x_checkpoint = static_cast<uint16_t>(cp.x_px);
            info.y_checkpoint = static_cast<uint16_t>(cp.y_px);
            
            float dx = cp.x_px - info.x;
            float dy = cp.y_px - info.y;
            info.hint_angle_deg = std::atan2(dy, dx) * 180.0f / PI;
            info.distance_to_checkpoint = std::sqrt(dx*dx + dy*dy);
        }
        
        info.checkpoints_remaining = static_cast<uint16_t>(track.checkpoint_count - p.current_checkpoint);
        
        RankInfo rank;
        rank.player_id = pid;
        rank.current_checkpoint = p.current_checkpoint;
        rank.distance_to_next = info.distance_to_checkpoint;
        ranking.push_back(rank);
        
        ticks.push_back(info);
    }
    
    calculate_ranking_positions(ticks, ranking);
    return ticks;
}

bool Race::compare_rank(const RankInfo& a, const RankInfo& b) {
    if (a.current_checkpoint != b.current_checkpoint) {
        return a.current_checkpoint > b.current_checkpoint;
    }
    return a.distance_to_next < b.distance_to_next;
}

void Race::calculate_ranking_positions(std::vector<PlayerTickInfo>& ticks, std::vector<RankInfo>& ranking) const {
    std::sort(ranking.begin(), ranking.end(), compare_rank);
    
    for (size_t i = 0; i < ranking.size(); ++i) {
        for (auto& tick : ticks) {
            if (tick.player_id == ranking[i].player_id) {
                tick.position_in_race = static_cast<uint16_t>(i + 1);
                break;
            }
        }
    }
}

RaceResult Race::build_race_results() const {
    RaceResult result;
    std::vector<std::pair<size_t, float>> times;
    
    for (const auto& kv : parts) {
        times.emplace_back(kv.first, kv.second.finish_time_seconds);
    }
    
    std::sort(times.begin(), times.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });
    
    uint8_t position = 1;
    for (const auto& t : times) {
        PlayerRaceResult pr;
        pr.player_id = t.first;
        pr.finish_time_seconds = t.second;
        pr.position = position++;
        result.result.push_back(pr);
    }
    
    return result;
}

void Race::clear_cars() {
    for (auto& kv : cars) {
        physics.destroy_body(kv.first);
    }
    cars.clear();
    cars_by_player.clear();
    parts.clear();
}

void Race::set_player_ptr(size_t player_id, Player* player_ptr) {
    players_by_id[player_id] = player_ptr;
}

void Race::apply_cheat(size_t playerId, uint8_t cheat_code) {
    if (cheat_code == CHEAT_INFINITE_LIFE) {
        auto it = cars.find(playerId);
        if (it != cars.end() && it->second) {
            it->second->set_infinite_life(true);
            std::cout << "[Race] Player " << playerId << " activated infinite life\n";
        }
    } else if (cheat_code == CHEAT_TELEPORT_NEXT_CHECKPOINT) {
        teleport_to_next_checkpoint(playerId);
    } else if (cheat_code == CHEAT_WIN_RACE) {
        cheat_win_race(playerId);
    }
}

void Race::teleport_to_next_checkpoint(size_t playerId) {
    auto it = parts.find(playerId);
    if (it == parts.end()) return;
    
    RaceParticipant& p = it->second;
    if (p.next_checkpoint_idx >= track.checkpoints.size()) return;
    
    const auto& cp = track.checkpoints[p.next_checkpoint_idx];
    auto car_it = cars.find(playerId);
    if (car_it != cars.end() && car_it->second) {
        b2Body* body = car_it->second->get_body();
        if (body) {
            body->SetTransform(b2Vec2(cp.x_px / PPM, cp.y_px / PPM), body->GetAngle());
            body->SetLinearVelocity(b2Vec2(0, 0));
            std::cout << "[Race] Player " << playerId << " teleported to checkpoint " 
                      << p.next_checkpoint_idx << "\n";
        }
    }
}

void Race::cheat_win_race(size_t playerId) {
    auto it = parts.find(playerId);
    if (it == parts.end()) return;
    
    RaceParticipant& p = it->second;
    p.current_checkpoint = track.checkpoint_count - 1;
    p.next_checkpoint_idx = track.checkpoint_count;
    p.state = ParticipantState::Finished;
    p.finish_time_seconds = race_duration;
    
    std::cout << "[Race] Player " << playerId << " used WIN_RACE cheat\n";
}
