#ifndef SERVER_MSG_H
#define SERVER_MSG_H

#include <cstdint>
#include <string>
#include <vector>

#include "room_info.h"
#include "player_info.h"
#include "car_info.h"
#include "results_info.h"
#include "map_tick_info.h"
#include "../enum/car_improvement.h" 

struct ServerMessage {
    enum class Type {
        Unknown,
        Ok,
        Pos,
        Rooms,
        RoomCreated,
        YourId,
        PlayerName,
        PlayersList,
        GameOver,
        MapInfo,
        RaceStart, // para indicar inicio de carrera
        Empty,
        Results, // resultados de carrera
        ImprovementOK,
        MarketTime
    };

    Type type;
    uint32_t id;
    int16_t x;
    int16_t y;
    float angle;
    std::string username;
    uint8_t map_id; // id del mapa recibido en RACE_START
    std::vector<RoomInfo> rooms;
    std::vector<PlayerInfo> players;
    uint8_t room_id;
    // MapInfo payload
    std::vector<PlayerTickInfo> players_tick;
    std::vector<NpcTickInfo> npcs_tick;
    std::vector<EventInfo> events_tick;
    TimeTickInfo race_time;

    // resultados de carrera
    std::vector<PlayerResultCurrent> results_current;
    std::vector<PlayerResultTotal> results_total;
    // Improvement ACK payload
    uint8_t improvement_id{0};
    uint8_t improvement_success{0};
    uint32_t improvement_total_penalty_seconds{0};

    ServerMessage();
};

enum class ServerOutType : uint8_t {
    Ok,
    Pos,
    YourId,
    PlayerName,
    Rooms,
    RoomCreated,
    PlayersList,
    CarList,
    RaceStart,
    Results,
    MapInfo,
    ImprovementOk,
    MarketTime
};

struct ServerOutMsg {
    ServerOutType type = ServerOutType::Ok;

    // Payloads básicos
    uint32_t id{0};
    int16_t x{0};
    int16_t y{0};
    float angle{0.f};
    uint32_t your_id{0};
    std::string username;
    uint8_t room_id{0};

    // Listas
    std::vector<RoomInfo> rooms;
    std::vector<PlayerInfo> players;
    std::vector<CarInfo> cars;

    // RaceStart
    uint8_t map_id{0}; // id del mapa para RaceStart
    std::vector<std::pair<int32_t, int32_t>> checkpoints;

    // Results
    std::vector<PlayerResultCurrent> results_current;
    std::vector<PlayerResultTotal> results_total;

    // MapInfo
    std::vector<PlayerTickInfo> players_tick;
    std::vector<NpcTickInfo> npcs_tick;
    std::vector<EventInfo> events_tick;
    TimeTickInfo race_time;

    uint16_t active_nitros{0};
    uint8_t nitro_msg{0};

    // ImprovementOK
    uint8_t improvement_id{0};
    uint32_t total_penalty_seconds{0};
    uint8_t improvement_success{0};

    // MarketTime
    TimeTickInfo market_time;

    ServerOutMsg();
};

#endif
