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

    bool is_recv_closed() const { return skt.is_stream_recv_closed(); }
    void shutdown(int mode) { skt.shutdown(mode); }

    ServerProtocol(const ServerProtocol&) = delete;
    ServerProtocol& operator=(const ServerProtocol&) = delete;

    ServerProtocol(ServerProtocol&&) = default;
    ServerProtocol& operator=(ServerProtocol&&) = default;

    ~ServerProtocol() = default;
};

#endif
