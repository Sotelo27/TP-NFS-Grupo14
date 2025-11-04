#include "physics_world.h"
#include <iostream>
#include <algorithm>
#include <cmath>

PhysicsWorld::PhysicsWorld() : world(b2Vec2(0.0f, 0.0f)) {}

void PhysicsWorld::create_car_body(size_t id, int16_t x_units, int16_t y_units, const CarModel& spec) {
    destroy_body(id);

    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.fixedRotation = false;
    def.position.Set(toMeters(x_units), toMeters(y_units));
    b2Body* body = world.CreateBody(&def);

    // Activar CCD para reducir "tunneling" a altas velocidades contra colisionadores delgados
    body->SetBullet(true);

    b2PolygonShape box;
    // Aproximar el tamaño físico del auto a ~2.5m x 2.5m (80px con PPM=32)
    // Si se agregan dimensiones al CarModel, usar esos valores aquí.
    float halfW = 1.25f;
    float halfL = 1.25f;
    box.SetAsBox(halfW, halfL);

    b2FixtureDef fdef;
    fdef.shape = &box;
    fdef.density = (spec.masaKg > 0.f) ? spec.masaKg / 1000.f : 1.f; // densidad por ahora.
    fdef.friction = 0.9f;
    fdef.restitution = 0.0f;
    body->CreateFixture(&fdef);

    body->SetLinearDamping(spec.dampingLineal);
    body->SetAngularDamping(spec.dampingAngular);

    bodies[id] = body;
}

void PhysicsWorld::destroy_body(size_t id) {
    auto it = bodies.find(id);
    if (it != bodies.end()) {
        world.DestroyBody(it->second);
        bodies.erase(it);
    }
}

void PhysicsWorld::step(float dt) {
    const float maxStep = 1.0f / 60.0f;
    int steps = static_cast<int>(std::ceil(dt / maxStep));
    if (steps < 1) steps = 1;
    const float h = dt / static_cast<float>(steps);
    for (int i = 0; i < steps; ++i) {
        world.Step(h, 8, 3);
    }
}

void PhysicsWorld::apply_force_center(size_t id, float fx, float fy) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return;
    it->second->ApplyForceToCenter(b2Vec2(fx, fy), true);
}

void PhysicsWorld::apply_torque(size_t id, float torque) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return;
    it->second->ApplyTorque(torque, true);
}

float PhysicsWorld::get_angle(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return 0.f;
    return it->second->GetAngle();
}

Pose PhysicsWorld::get_pose(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return Pose{0, 0, 0.f};
    b2Vec2 p = it->second->GetPosition();
    return Pose{toUnits(p.x), toUnits(p.y), it->second->GetAngle()};
}

void PhysicsWorld::cap_linear_speed(size_t id, float max_mps) {
    auto it = bodies.find(id);
    if (it == bodies.end()) return;
    b2Vec2 v = it->second->GetLinearVelocity();
    float speed = v.Length();
    if (speed > max_mps && speed > 0.f) {
        v *= (max_mps / speed);
        it->second->SetLinearVelocity(v);
    }
}

float PhysicsWorld::get_linear_speed(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return 0.f;
    b2Vec2 v = it->second->GetLinearVelocity();
    return v.Length();
}

void PhysicsWorld::clear_static_geometry() {
    for (b2Body* b : static_bodies) {
        if (b) world.DestroyBody(b);
    }
    static_bodies.clear();
}

void PhysicsWorld::load_static_geometry(const MapConfig& cfg) {
    //antes de cargar limpio, servira cuando se cambie de mapa
    clear_static_geometry();

    const float PPM = (cfg.pixels_per_meter > 0.f) ? cfg.pixels_per_meter : 32.f;
    const float pixel_to_meters = 1.0f / PPM;
    std::cout << "[Physics] Loading static geometry: PPM=" << PPM
              << " rects=" << cfg.rects.size()
              << " polylines=" << cfg.polylines.size() << "\n";

    // cARGO los rectaugos del mapa
    size_t ridx = 0;
    for (const auto& r : cfg.rects) {
        add_static_rect_body_px(r, pixel_to_meters);
        if (ridx < 8) {
            const float cx_m = (r.x_px + r.w_px * 0.5f) * pixel_to_meters;
            const float cy_m = (r.y_px + r.h_px * 0.5f) * pixel_to_meters;
            std::cout << "[Physics] Rect[" << ridx << "] px=(x=" << r.x_px << ", y=" << r.y_px
                      << ", w=" << r.w_px << ", h=" << r.h_px
                      << ") rot_deg=" << r.rotation_deg
                      << " center_m=(" << cx_m << ", " << cy_m << ")\n";
        }
        ++ridx;
    }

    // Cargo las polilinas del mapa, solo son 4 paredes 
    size_t pidx = 0;
    for (const auto& pl : cfg.polylines) {
        if (pl.points_px.size() < 2) continue;
        auto verts = make_polyline_vertices_m(pl, pixel_to_meters);
        add_static_polyline_bodies_px(pl, pixel_to_meters);
        if (pidx < 8 && !verts.empty()) {
            float minx = verts[0].x, miny = verts[0].y, maxx = verts[0].x, maxy = verts[0].y;
            for (const auto& v : verts) {
                minx = std::min(minx, v.x); maxx = std::max(maxx, v.x);
                miny = std::min(miny, v.y); maxy = std::max(maxy, v.y);
            }
            std::cout << "[Physics] Polyline[" << pidx << "] verts=" << verts.size()
                      << " loop=" << (pl.loop ? 1 : 0)
                      << " bbox_m=[(" << minx << "," << miny << ")- (" << maxx << "," << maxy << ")]\n";
        }
        ++pidx;
    }
}

// Crea un rectangulo que simula el edificio a partir de medidas en pixeles
// pixel_to_meters = 1 / PPM  (factor para convertir px en metros)
void PhysicsWorld::add_static_rect_body_px(const RectCollider& rect, float pixel_to_meters) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;

    // Box2D posiciona por el centro: calculamos el centro en metros
    const float centerX_m = (rect.x_px + rect.w_px * 0.5f) * pixel_to_meters;
    const float centerY_m = (rect.y_px + rect.h_px * 0.5f) * pixel_to_meters;
    bodyDef.position.Set(centerX_m, centerY_m);

    b2Body* body = world.CreateBody(&bodyDef);

    // Debug, luego capaz lo borre si soluciono esto, estoy achicando el cuerpo para evitar colisiones tempranas 
    const float halfWidth_px  = std::max(0.0f, rect.w_px * 0.5f - rect_collider_margin_px);
    const float halfHeight_px = std::max(0.0f, rect.h_px * 0.5f - rect_collider_margin_px);

    b2PolygonShape boxShape;
    const float halfWidth_m  = halfWidth_px  * pixel_to_meters;
    const float halfHeight_m = halfHeight_px * pixel_to_meters;
    const float angle_rad = rect.rotation_deg * b2_pi / 180.0f;

    // La forma se rota alrededor del origen del body (el centro del rectángulo)
    boxShape.SetAsBox(std::max(halfWidth_m, 0.001f),
                      std::max(halfHeight_m, 0.001f),
                      b2Vec2(0.0f, 0.0f),
                      angle_rad);

    b2FixtureDef fixtureDef;
    fixtureDef.shape       = &boxShape;
    fixtureDef.friction    = rect.friction;
    fixtureDef.restitution = rect.restitution;
    fixtureDef.isSensor    = rect.is_sensor;

    body->CreateFixture(&fixtureDef);
    static_bodies.push_back(body);
}


// Convierte puntos de una polilinea desde pixeles absolutos a metros
std::vector<b2Vec2> PhysicsWorld::make_polyline_vertices_m(const PolylineCollider& poly, float pixel_to_meters) const {
    std::vector<b2Vec2> vertices_m;
    vertices_m.reserve(poly.points_px.size());

    for (auto [localX_px, localY_px] : poly.points_px) {
        const float x_m = (poly.x_px + localX_px) * pixel_to_meters;
        const float y_m = (poly.y_px + localY_px) * pixel_to_meters;
        vertices_m.emplace_back(x_m, y_m);
    }
    return vertices_m;
}

void PhysicsWorld::compute_ghost_vertices(const std::vector<b2Vec2>& verts, b2Vec2& ghostA, b2Vec2& ghostB) const {
    const int n = static_cast<int>(verts.size());
    if (n <= 1) { ghostA = verts.front(); ghostB = verts.back(); return; }
    ghostA = verts[0];
    ghostB = verts[n - 1];
}

void PhysicsWorld::add_static_polyline_bodies_px(const PolylineCollider& pl, float pixel_to_meters) {
    if (pl.points_px.size() < 2) return;

    auto verts = make_polyline_vertices_m(pl, pixel_to_meters);

    b2BodyDef bd;
    bd.type = b2_staticBody;
    b2Body* body = world.CreateBody(&bd);

    b2FixtureDef fd;
    fd.friction = pl.friction;
    fd.restitution = pl.restitution;
    fd.isSensor = pl.is_sensor;

    if (pl.loop) {
        b2ChainShape chain;
        chain.CreateLoop(verts.data(), static_cast<int>(verts.size()));
        fd.shape = &chain;
        body->CreateFixture(&fd);
    } else {
        b2Vec2 ghostA, ghostB;
        compute_ghost_vertices(verts, ghostA, ghostB);

        {
            b2ChainShape chain;
            chain.CreateChain(verts.data(), static_cast<int32>(verts.size()), ghostA, ghostB);
            fd.shape = &chain;
            body->CreateFixture(&fd);
        }
        {
            std::vector<b2Vec2> revVerts(verts.rbegin(), verts.rend());
            b2Vec2 ghostRA, ghostRB;
            compute_ghost_vertices(revVerts, ghostRA, ghostRB);
            b2ChainShape chain2;
            chain2.CreateChain(revVerts.data(), static_cast<int32>(revVerts.size()), ghostRA, ghostRB);
            fd.shape = &chain2;
            body->CreateFixture(&fd);
        }
    }

    static_bodies.push_back(body);
}
