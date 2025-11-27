#include "physics_world.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdint>

#define PPM 32.f

PhysicsWorld::PhysicsWorld() : world(b2Vec2(0.0f, 0.0f)), contact_listener() {
    world.SetContactListener(&contact_listener);
}

void PhysicsWorld::create_car_body(size_t id, float x_meters, float y_meters, const CarModel& spec) {
    destroy_body(id);

    b2BodyDef def;
    def.type = b2_dynamicBody;
    def.fixedRotation = false;
    def.position.Set(x_meters, y_meters);
    b2Body* body = world.CreateBody(&def);
    body->SetBullet(true);

    b2PolygonShape box;
    float halfW = 0.45f;
    float halfL = 0.45f;
    box.SetAsBox(halfW, halfL);

    //calculo de area: Área del box (ancho * alto)
    float width  = 2.0f * halfW;
    float height = 2.0f * halfL;
    float area   = width * height;

    float density = spec.masaKg / area; // masa / area


    b2FixtureDef fdef;
    fdef.shape = &box;
    fdef.density = density;
    fdef.friction = 0.8f;
    fdef.restitution = 0.0f;
    body->CreateFixture(&fdef);

    body->SetLinearDamping(spec.dampingLineal);
    body->SetAngularDamping(spec.dampingAngular);

    bodies[id] = body;
}

b2Body* PhysicsWorld::get_body(size_t id) const {
    auto it = bodies.find(id);
    if (it == bodies.end()) return nullptr;
    return it->second;
}

void PhysicsWorld::destroy_body(size_t id) {
    auto it = bodies.find(id);
    if (it != bodies.end()) {
        world.DestroyBody(it->second);
        bodies.erase(it);
    }
}

void PhysicsWorld::step(float dt) {
    constexpr int velocityIterations = 8;
    constexpr int positionIterations = 3;
    world.Step(dt, velocityIterations, positionIterations);
}

std::vector<CheckpointEvent> PhysicsWorld::consume_checkpoint_events() {
    return contact_listener.consume_checkpoint_events();
}

void PhysicsWorld::clear_static_geometry() {
    for (b2Body* b : static_bodies) {
        if (b) world.DestroyBody(b);
    }
    static_bodies.clear();
    static_entities.clear();
    next_static_id_ = 1;
}

void PhysicsWorld::load_static_geometry(const MapConfig& cfg) {
    //antes de cargar limpio, servira cuando se cambie de mapa
    clear_static_geometry();

    const float pixel_to_meters = 1.0f / PPM;

    //----------------------------------------------- 
    // cARGO los rectaugos del mapa
    size_t ridx = 0;
    for (const auto& r : cfg.rects) {
        add_static_rect_body_px(r, pixel_to_meters);
        ++ridx;
    }

    // Cargo las polilinas del mapa, solo son 4 paredes 
    size_t pidx = 0;
    for (const auto& pl : cfg.polylines) {
        if (pl.points_px.size() < 2) continue;
        auto verts = make_polyline_vertices_m(pl, pixel_to_meters);
        add_static_polyline_bodies_px(pl, pixel_to_meters);
        ++pidx;
    }

    if (!cfg.checkpoints.empty()) {
        for (const auto& [route_id, cps_vec] : cfg.checkpoints) {
            size_t cidx = 0;
            for (const auto& cp : cps_vec) {
                add_checkpoint_body_px(cp, pixel_to_meters);
                ++cidx;
            }
        }
    } else {
        std::cout << "[Physics] No checkpoints found in MapConfig\n";
    }
}

void PhysicsWorld::add_static_rect_body_px(const RectCollider& rect, float pixel_to_meters) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;

    // Box2D posiciona por el centro: calculamos el centro en metros
    const float centerX_m = (rect.x_px + rect.w_px * 0.5f) * pixel_to_meters;
    const float centerY_m = (rect.y_px + rect.h_px * 0.5f) * pixel_to_meters;
    bodyDef.position.Set(centerX_m, centerY_m);

    bodyDef.angle = rect.rotation_deg * b2_pi / 180.0f;

    b2Body* body = world.CreateBody(&bodyDef);

    b2PolygonShape boxShape;
    const float halfWidth_m  = (rect.w_px * 0.5f) * pixel_to_meters;
    const float halfHeight_m = (rect.h_px * 0.5f) * pixel_to_meters;

    boxShape.SetAsBox(halfWidth_m, halfHeight_m);

    b2FixtureDef fixtureDef;
    fixtureDef.shape       = &boxShape;
    fixtureDef.friction    = rect.friction;
    fixtureDef.restitution = rect.restitution;
    fixtureDef.isSensor    = rect.is_sensor;

    body->CreateFixture(&fixtureDef);

    // añado el edificio al user data del body
    auto ent = std::make_unique<BuildingEntity>(next_static_id_++, body);
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ent.get());
    static_entities.push_back(std::move(ent));

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

void PhysicsWorld::add_static_polyline_bodies_px(const PolylineCollider& pl, float pixel_to_meters) {
    // Necesitamos al menos 2 puntos
    if (pl.points_px.size() < 2) return;

    // Convertir puntos de pixeles → metros
    auto verts = make_polyline_vertices_m(pl, pixel_to_meters);

    //creo body pero en estatico
    b2BodyDef bd;
    bd.type = b2_staticBody;
    b2Body* body = world.CreateBody(&bd);

    // Definir propiedades del fixture
    b2FixtureDef fd;
    fd.friction    = pl.friction;
    fd.restitution = pl.restitution;
    fd.isSensor    = pl.is_sensor;

    // Forma chain (borde)
    b2ChainShape chain;

    if (pl.loop) {
        // Perimetro cerrado
        chain.CreateLoop(verts.data(), (int32)(verts.size()));
    } else {
        // (lado A)
        b2Vec2 prevF = verts.front();
        b2Vec2 nextF = verts.back();

        b2ChainShape chainFwd;
        chainFwd.CreateChain(verts.data(), (int32)verts.size(), prevF, nextF);
        fd.shape = &chainFwd;
        body->CreateFixture(&fd);

        // (lado B)
        std::vector<b2Vec2> revVerts(verts.rbegin(), verts.rend());
        b2Vec2 prevR = revVerts.front();
        b2Vec2 nextR = revVerts.back();

        b2ChainShape chainRev;
        chainRev.CreateChain(revVerts.data(), (int32)revVerts.size(), prevR, nextR);
        fd.shape = &chainRev;
        body->CreateFixture(&fd);
    }

    // Crear entidad BorderEntity
    auto ent = std::make_unique<BorderEntity>(next_static_id_++, body);
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ent.get());
    static_entities.push_back(std::move(ent));

    static_bodies.push_back(body);
}


void PhysicsWorld::add_checkpoint_body_px(const Checkpoint& cp, float pixel_to_meters) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;

    // Centro del checkpoint considerando rotacion
    // px = (x, y) + R(a) * (w/2, h/2)
    const float a = cp.rotation_deg * b2_pi / 180.0f;
    const float halfW_px = cp.w_px * 0.5f;
    const float halfH_px = cp.h_px * 0.5f;
    const float cx_px = cp.x_px + std::cos(a) * halfW_px - std::sin(a) * halfH_px;
    const float cy_px = cp.y_px + std::sin(a) * halfW_px + std::cos(a) * halfH_px;
    const float centerX_m = cx_px * pixel_to_meters;
    const float centerY_m = cy_px * pixel_to_meters;
    bodyDef.position.Set(centerX_m, centerY_m);

    // Rotacion del body
    bodyDef.angle = cp.rotation_deg * b2_pi / 180.0f;

    b2Body* body = world.CreateBody(&bodyDef);

    // Caja en metros
    b2PolygonShape boxShape;
    const float halfWidth_m  = (cp.w_px * 0.5f) * pixel_to_meters;
    const float halfHeight_m = (cp.h_px * 0.5f) * pixel_to_meters;
    boxShape.SetAsBox(halfWidth_m, halfHeight_m);

    b2FixtureDef fixtureDef;
    fixtureDef.shape       = &boxShape;
    fixtureDef.friction    = 0.0f;
    fixtureDef.restitution = 0.0f;
    fixtureDef.isSensor    = cp.is_sensor;

    body->CreateFixture(&fixtureDef);

    auto ent = std::make_unique<CheckpointEntity>(
        next_static_id_++,
        body,
        cp.index,
        cp.race_id,
        cp.type
    );
    body->GetUserData().pointer = reinterpret_cast<uintptr_t>(ent.get());
    static_entities.push_back(std::move(ent));
    static_bodies.push_back(body);
}

