#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <utility>
#include <vector>

#include "../common/dto/player_info.h"
#include "../common/dto/room_info.h"
#include "../common/player_aux.h"
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
    Queue<ServerOutMsg> mensajes_a_enviar;
    ClientThreadRecv recv;
    ClientThreadSend send;
    bool recv_started{false};
    bool send_started{false};

public:
    /*
     * Constructor de ClientHandler que recibe un `Socket` ya conectado
     * con el client, el id del client y una referencia a la cola de
     * actiones de clients, donde se pushearán las peticiones recibidas.
     */
    explicit ClientHandler(Socket&& skt_client, size_t id, Queue<ClientAction>& actiones_clients);

    /*
     * Con `ClientHandler::ejecutar` se inician los hilos de recepción
     * y envío de mensajes.
     */
    void ejecutar();

    /*
     * Inicia solo el hilo de recepcion (para estado pendiente en lobby)
     */
    void start_recv_only();

    /*
     * Inicia solo el hilo de envio (al entrar a una sala)
     */
    void start_send_only();

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

    // Enviar listado de salas a este cliente
    void send_rooms_to_client(const std::vector<RoomInfo>& rooms);

    // Enviar OK a este cliente
    void send_ok_to_client();

    // Enviar al cliente su propio id asignado en la sala
    void send_your_id_to_client(uint32_t id);

    // Enviar el username de un player (id, username)
    void send_player_name_to_client(uint32_t id, const std::string& username);

    // Notificar id de sala recién creada a este cliente
    void send_room_created_to_client(uint8_t room_id);
    void send_players_list_to_client(const std::vector<PlayerInfo>& players); // NUEVO
    
    /*
     * Envia la informacion del mapa (jugadores, NPCs, eventos) en tiempo de juego al cliente
     */
    void send_map_info_to_client(const std::vector<PlayerTickInfo>& players,
                                 const std::vector<NpcTickInfo>& npcs,
                                 const std::vector<EventInfo>& events,
                                 TimeTickInfo time_info);

    /*
     * Encola el envío de una posición (id,x,y,angle) al cliente
     */
    void server_enviar_pos(uint32_t id, int16_t x, int16_t y, float angle);

    /*
     * Envia las posiciones de todos los jugadores al cliente.
     */
    void send_positions_to_all(const std::vector<PlayerPos>& positions);
    void send_race_start(uint8_t map_id, const std::vector<std::pair<int32_t, int32_t>>& checkpoints, uint32_t tiempo_partida);

    /*
     * Envia resultados de carrera (vector current) a este cliente
     */ 
    void send_results_to_client(const std::vector<PlayerResultCurrent>& current);

    /*
     * Envia el ok de compra de mejora al cliente
     */
    void send_improvement_ok_to_client(uint32_t player_id, uint8_t improvement_id, bool success, uint32_t total_penalty_seconds);

    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;

    ClientHandler(ClientHandler&&) = default;
    ClientHandler& operator=(ClientHandler&&) = default;

    ~ClientHandler();
};

#endif
