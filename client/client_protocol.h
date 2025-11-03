#ifndef CLIENT_PROTOCOL_H
#define CLIENT_PROTOCOL_H

#include <string>
#include <utility>

#include "../common/constants.h"
#include "../common/socket.h"
#include "../common/dto/movement.h"
#include "../common/dto/server_msg.h"

class ClientProtocol {
private:
    Socket skt;

public:
    explicit ClientProtocol(Socket&& skt): skt(std::move(skt)) {}

    // Send username (string)
    void send_name(const std::string& username);

    // Send movement (enum Movement)
    void send_move(Movement mov);

    // Rooms: crear / unirse
    void send_create_room();
    void send_join_room(uint8_t room_id);

    // Exit match
    void send_exit();

    // Receive() returns ServerMessage DTO
    ServerMessage receive();

    bool is_recv_closed() const { return skt.is_stream_recv_closed(); }
    void shutdown(int mode) { skt.shutdown(mode); }

    ClientProtocol(const ClientProtocol&) = delete;
    ClientProtocol& operator=(const ClientProtocol&) = delete;

    ClientProtocol(ClientProtocol&&) = default;
    ClientProtocol& operator=(ClientProtocol&&) = default;

    ~ClientProtocol() = default;
};

#endif
