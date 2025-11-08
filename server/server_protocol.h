#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include <cstdint>
#include <utility>
#include <vector>
#include <string>

#include "../common/constants.h"
#include "../common/socket.h"
#include "../common/dto/client_msg.h"
#include "../common/dto/room_info.h"
#include "../common/dto/car_info.h"
#include "../common/dto/results_info.h"
#include "../common/dto/map_tick_info.h"
#include "../common/dto/player_info.h"

class ServerProtocol {
private:
    Socket skt;

public:
    explicit ServerProtocol(Socket&& skt): skt(std::move(skt)) {}

    // Server: send OK
    void send_ok();

    // Server: send position (id,x,y,angle)
    void send_pos(uint32_t id, int16_t x, int16_t y, float angle);

    // Server: send your id to client
    void send_your_id(uint32_t id);

    // Server: send player username (player_id, length, username)
    void send_player_name(uint32_t id, const std::string& username);

    // Server: send listado de salas (id, current, max)
    void send_rooms(const std::vector<RoomInfo>& rooms);

    // Server: enviar id de sala recién creada
    void send_room_created(uint8_t room_id);

    // Server: enviar lista de jugadores en sala de espera
    void send_players_list(const std::vector<PlayerInfo>& players);

    // Server: receive() returns ClientMessage (base)
    ClientMessage receive();

    /*
     * Enviar mensaje genérico (nitro / mensajes) en el formato legacy.
     * Compatibilidad con la antigua API de ProtocolServer.
     */
    void enviar_mensaje(uint16_t cantidad_nitros_activos, uint8_t mensaje);

    /*
     * Enviar lista/paquete de rooms por defecto (compatibilidad).
     */
    void enviar_rooms_default();

    // --- Nuevos envíos S2C según enunciado ---
    void send_cars_list(const std::vector<CarInfo>& cars);

    // Race start
    // checkpoints opcional: pares (x,y) en 32 bits big endian (si no se usa, pasar vector vacío)
    void send_race_start(const std::string& map, uint8_t amount_checkpoints,
                         const std::vector<std::pair<int32_t,int32_t>>& checkpoints);

    void send_results(const std::vector<PlayerResultCurrent>& current,
                      const std::vector<PlayerResultTotal>& total);

    void send_map_info(const std::vector<PlayerTickInfo>& players,
                       const std::vector<NpcTickInfo>& npcs,
                       const std::vector<EventInfo>& events);

    bool is_recv_closed() const { return skt.is_stream_recv_closed(); }
    void shutdown(int mode) { skt.shutdown(mode); }

    ServerProtocol(const ServerProtocol&) = delete;
    ServerProtocol& operator=(const ServerProtocol&) = delete;

    ServerProtocol(ServerProtocol&&) = default;
    ServerProtocol& operator=(ServerProtocol&&) = default;

    ~ServerProtocol() = default;
};

#endif
