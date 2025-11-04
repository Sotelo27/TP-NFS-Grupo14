#ifndef CLIENT_THREAD_SEND_H
#define CLIENT_THREAD_SEND_H

#include "client_action.h"
#include "../common/queue.h"
#include "../common/thread.h"
#include "server_protocol.h"
#include <variant>
#include <vector>
#include <string>

// Mensaje de salida general del servidor -> cliente
enum class ServerOutType {
    Ok,
    Pos,
    YourId,
    PlayerName,
    Rooms,
    RoomCreated,
    CarList,
    RaceStart,
    Results,
    MapInfo
};

struct ServerOutMsg {
    ServerOutType type{ServerOutType::Ok};

    // Payloads
    // Pos
    uint32_t id{0};
    int16_t x{0};
    int16_t y{0};
    float angle{0.f};

    // YourId
    uint32_t your_id{0};

    // PlayerName
    std::string username;

    // Rooms
    std::vector<RoomInfo> rooms;

    // RoomCreated
    uint8_t room_id{0};

    // CarList
    std::vector<CarInfo> cars;

    // RaceStart
    std::string map_name;
    uint8_t amount_checkpoints{0};
    std::vector<std::pair<int32_t,int32_t>> checkpoints;

    // Results
    std::vector<PlayerResultCurrent> results_current;
    std::vector<PlayerResultTotal> results_total;

    // MapInfo
    std::vector<PlayerTickInfo> players_tick;
    std::vector<NpcTickInfo> npcs_tick;
    std::vector<EventInfo> events_tick;
};

class ClientThreadSend: public Thread {
private:
    ServerProtocol& protocol;
    size_t id = 0;
    Queue<ServerOutMsg>& mensajes_a_enviar;

public:
    /*
     * Constructor de ClientThreadSend que recibe una referencia al
     * protocol del client, el id del client y una referencia a la
     * cola de mensajes a enviar.
     */
    explicit ClientThreadSend(ServerProtocol& protocol, size_t id,
                             Queue<ServerOutMsg>& mensajes_a_enviar);

    void run() override;

    ClientThreadSend(const ClientThreadSend&) = delete;
    ClientThreadSend& operator=(const ClientThreadSend&) = delete;

    ClientThreadSend(ClientThreadSend&&) = default;
    ClientThreadSend& operator=(ClientThreadSend&&) = default;

    ~ClientThreadSend() = default;
};

#endif
