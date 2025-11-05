#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <list>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "client_handler.h"
#include "../common/dto/player_info.h"

class ClientListProtected {
private:
    std::list<std::shared_ptr<ClientHandler>> clients;
    std::mutex m;

public:
    ClientListProtected() = default;

    /*
     * Agrega un nuevo client a la list de clients.
     */
    void agregar_client(std::shared_ptr<ClientHandler> client);

    /*
     * Elimina de la list de clients a todos los clients que ya no estén
     * activos.
     *
     * Se pasa un vector donde se guardarán en él los IDs
     * de los clients que hayan sido eliminados.
     */
    void reap(std::vector<size_t>& clients_eliminados);

    /*
     * Elimina a todos los clients de la list.
     *
     * Se pasa un vector donde se guardarán en él los IDs
     * de los clients que hayan sido eliminados.
     */
    void clear(std::vector<size_t>& clients_eliminados);

    /*
     * Enviar una posición (x,y,angle) a un cliente específico por id.
     */
    void send_pos_to(size_t id, int16_t x, int16_t y, float angle);

    /*
     * Envia las posiciones de todos los jugadores a todos los clientes.
     */
    void broadcast_player_positions(const std::vector<PlayerPos>& positions);
    void broadcast_players_list(const std::vector<PlayerInfo>& players); // NUEVO

    ClientListProtected(const ClientListProtected&) = delete;
    ClientListProtected& operator=(const ClientListProtected&) = delete;

    ClientListProtected(ClientListProtected&&) = default;
    ClientListProtected& operator=(ClientListProtected&&) = default;

    ~ClientListProtected();
};

#endif
