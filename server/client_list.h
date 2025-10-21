#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <list>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "client_handler.h"

class ClientListProtected {
private:
    std::list<std::unique_ptr<ClientHandler>> clients;
    std::mutex m;

public:
    ClientListProtected() = default;

    /*
     * Agrega un nuevo client a la list de clients.
     */
    void agregar_client(std::unique_ptr<ClientHandler> client);

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
     * Envía a todos los clients un mensaje informando que se ha activado
     * un nitro, junto con la cantidad de nitros activos.
     */
    void broadcast_nitro_activado(uint16_t cantidad_nitros_activos);

    /*
     * Envía a todos los clients un mensaje informando que se ha desactivado
     * un nitro, junto con la cantidad de nitros activos.
     */
    void broadcast_nitro_desactivado(uint16_t cantidad_nitros_activos);

    ClientListProtected(const ClientListProtected&) = delete;
    ClientListProtected& operator=(const ClientListProtected&) = delete;

    ClientListProtected(ClientListProtected&&) = default;
    ClientListProtected& operator=(ClientListProtected&&) = default;

    ~ClientListProtected();
};

#endif
