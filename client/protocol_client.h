#ifndef PROTOCOL_CLIENT_H
#define PROTOCOL_CLIENT_H

#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "../common/constants.h"
#include "../common/socket.h"

struct DataClient {
    std::string action;
    std::vector<std::string> parametros;
};

class ProtocolClient {
private:
    Socket skt;
    DataClient data_client;

    void decodificar_action_server(uint8_t action, std::string& salida);

public:
    /*
     * Constructor de `ProtocolClient` que recibe un `Socket` *ya* conectado.
     */
    explicit ProtocolClient(Socket&& skt);

    /*
     * Con `ProtocolClient::client_nitro` se envía una petición al servidor para activar el
     * nitro.
     */
    void client_nitro();

    /*
     * Con `ProtocolClient::recibir` se recibe un único mensaje enviado por el servidor.
     *
     * El método es bloqueante, es decir, esperará hasta que llegue un mensaje.
     */
    const DataClient& recibir();

    ProtocolClient(const ProtocolClient&) = delete;
    ProtocolClient& operator=(const ProtocolClient&) = delete;

    ProtocolClient(ProtocolClient&&) = default;
    ProtocolClient& operator=(ProtocolClient&&) = default;

    ~ProtocolClient() = default;
};

#endif
