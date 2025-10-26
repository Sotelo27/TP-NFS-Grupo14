#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <utility>

#include "../common/queue.h"
#include "../common/thread.h"

#include "client_action.h"
#include "client_thread_recv.h"
#include "client_thread_send.h"
#include "server_protocol.h"

class ClientHandler {
private:
    ServerProtocol protocol;
    size_t id = 0;
    Queue<server_msg_pos> mensajes_a_enviar;
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
     * Encola el envío de una posición (x,y) al cliente (Pair A: CODE_S2C_POS).
     */
    void server_enviar_pos(int16_t x, int16_t y);

    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;

    ClientHandler(ClientHandler&&) = default;
    ClientHandler& operator=(ClientHandler&&) = default;

    ~ClientHandler();
};

#endif
