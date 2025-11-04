#ifndef SERVER_HANDLER_H
#define SERVER_HANDLER_H

#include <string>
#include <utility>

#include "../../common/queue.h"
#include "../../common/thread.h"
#include "../client_protocol.h"

#include "server_thread_recv.h"
#include "server_thread_send.h"

class ServerHandler {
private:
    ClientProtocol protocol;
    Queue<client_msg_pos> messages_send;
    ServerThreadRecv recv;
    ServerThreadSend send;

public:
    /*
     * Constructor de ServerHandler que recibe un `Socket` ya conectado
     * con el Server, el id del Server y una referencia a la cola de
     * actiones de Servers, donde se pushearán las peticiones recibidas.
     */
    explicit ServerHandler(Socket&& skt, Queue<ServerMessage>& server_actions);

    /*
     * Con `ServerHandler::ejecutar` se inician los hilos de recepción
     * y envío de mensajes.
     */
    void start();

    /*
     * Con `ServerHandler::is_alive` se puede consultar si ambos hilos
     * (recepción y envío) están activos.
     *
     * Retorna `true` si ambos hilos están activos, `false` en caso
     * contrario.
     */
    bool is_alive();

    /*
     * Con `ServerHandler::hard_kill` se detienen ambos hilos y se
     * cierra el canal de recepción y envío del socket.
     */
    void hard_kill();

    void send_movement(Movement mov);

    void send_username(const std::string& username);

    void send_create_room();

    ServerHandler(const ServerHandler&) = delete;
    ServerHandler& operator=(const ServerHandler&) = delete;

    ServerHandler(ServerHandler&&) = default;
    ServerHandler& operator=(ServerHandler&&) = default;

    ~ServerHandler();
};

#endif
