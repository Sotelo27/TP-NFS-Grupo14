#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <utility>

#include "../common/queue.h"
#include "../common/thread.h"

#include "client_action.h"
#include "client_thread_recv.h"
#include "client_thread_send.h"
#include "protocol_server.h"

class ClientHandler {
private:
    ProtocolServer protocol;
    size_t id = 0;
    Queue<server_msg> mensajes_a_enviar;
    ClientThreadRecv recv;
    ClientThreadSend send;

public:
    /*
     * Constructor de ClientHandler que recibe un `Socket` ya conectado
     * con el client, el id del client y una referencia a la cola de
     * actiones de clients, donde se pushearán las peticiones recibidas.
     */
    explicit ClientHandler(Socket&& skt_client, size_t id,
                            Queue<ClientAction>& actiones_clients);

    /*
     * Con `ClientHandler::ejecutar` se inician los hilos de recepción
     * y envío de mensajes.
     */
    void ejecutar();

    /*
     * Con `ClientHandler::is_alive` se puede consultar si ambos hilos
     * (recepción y envío) están activos.
     *
     * Retorna `true` si ambos hilos están activos, `false` en caso
     * contrario.
     */
    bool is_alive();

    /*
     * Con `ClientHandler::hard_kill` se detienen ambos hilos y se
     * cierra el canal de recepción y envío del socket.
     */
    void hard_kill();

    /*
     * Con `ClientHandler::get_id` se puede obtener el id del client
     * manejado por este handler.
     */
    size_t get_id();

    /*
     * Con `ClientHandler::server_enviar` se puede enviar un mensaje
     * al client.
     */
    void server_enviar(uint16_t cantidad_nitros_activos, uint8_t mensaje);

    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;

    ClientHandler(ClientHandler&&) = default;
    ClientHandler& operator=(ClientHandler&&) = default;

    ~ClientHandler();
};

#endif
