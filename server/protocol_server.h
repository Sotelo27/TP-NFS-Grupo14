#ifndef PROTOCOL_SERVER_H
#define PROTOCOL_SERVER_H

#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "../common/socket.h"

class ProtocolServer {
private:
    Socket skt;

public:
    /*
     * Constructor de `ProtocolServer` que recibe un `Socket` *ya* conectado.
     * */
    explicit ProtocolServer(Socket&& skt);

    /*
     * Con `ProtocolServer::server_receive` se puede recibir el código
     * de la acción que el client quiere realizar.
     *
     * El método retornara luego de haber recibido el código.
     */
    uint8_t recibir();

    /*
     * Con `ProtocolServer::enviar_mensaje` se puede enviar la cantidad de nitros
     * activos y un mensaje, en formato uint8_t, al client.
     */
    void enviar_mensaje(uint16_t cantidad_nitros_activos, uint8_t mensaje);

    /*
     * Con `ProtocolServer::canal_recibir_cerrado` se puede consultar
     * si el canal de recepción del socket fue cerrado.
     *
     * Retorna `true` si el canal de recepción del socket fue cerrado,
     * `false` en caso contrario.
     */
    bool canal_recibir_cerrado() const;

    /*
     * Con `ProtocolServer::shutdown` se puede cerrar el canal de
     * recepción, el canal de envío o ambos.
     */
    void shutdown(int modo);

    ProtocolServer(const ProtocolServer&) = delete;
    ProtocolServer& operator=(const ProtocolServer&) = delete;

    ProtocolServer(ProtocolServer&&) = default;
    ProtocolServer& operator=(ProtocolServer&&) = default;

    ~ProtocolServer() = default;
};

#endif
