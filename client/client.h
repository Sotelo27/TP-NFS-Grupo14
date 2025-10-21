#ifndef CLIENT_H
#define CLIENT_H

#include <optional>
#include <string>

#include "protocol_client.h"

class Client {
private:
    ProtocolClient protocol;

    void procesar_respuestas(int cantidad);
    void procesar_respuesta(const DataClient& respuesta);

    bool parsear_comando(const std::string& entrada, std::string& action, std::string& argumento);
    void validar_numero(const std::string& str);

public:
    /*
     * Constructor de `client` que recibe un `Socket` *ya* conectado al servidor.
     */
    explicit Client(Socket&& skt);

    /*
     * Con `client::procesar_action` se envía una acción al servidor.
     *
     * Si la acción requiere un parámetro, este debe ser pasado
     * en el segundo argumento.
     */
    void procesar_action(const std::string& action,
                         const std::optional<std::string>& parametro = std::nullopt);

    /*
     * Con `client::procesar_actiones` se procesan las actiones
     * leídas desde la entrada estandar.
     */
    void procesar_actiones();

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    ~Client() = default;
};

#endif
