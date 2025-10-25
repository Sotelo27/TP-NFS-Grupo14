#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include <cstdint>
#include <utility>

#include "../common/constants.h"
#include "../common/socket.h"
#include "../common/base_protocol.h"

class ServerProtocol {
private:
    Socket skt;

public:
    explicit ServerProtocol(Socket&& skt): skt(std::move(skt)) {}

    // Server: send OK
    void send_ok();

    // Server: send position (x,y)
    void send_pos(int16_t x, int16_t y);

    // Server: receive() returns ClientMessage (base)
    ClientMessage receive();

    bool is_recv_closed() const { return skt.is_stream_recv_closed(); }
    void shutdown(int mode) { skt.shutdown(mode); }

    ServerProtocol(const ServerProtocol&) = delete;
    ServerProtocol& operator=(const ServerProtocol&) = delete;

    ServerProtocol(ServerProtocol&&) = default;
    ServerProtocol& operator=(ServerProtocol&&) = default;

    ~ServerProtocol() = default;
};

#endif
