#include "client_thread_recv.h"

#include <utility>

ClientThreadRecv::ClientThreadRecv(ProtocolServer& protocol, size_t id,
                                 Queue<ClientAction>& actiones_clients):
        protocol(protocol), id(id), actiones_clients(actiones_clients) {}

void ClientThreadRecv::run() {
    while (should_keep_running()) {
        try {
            uint8_t action = protocol.recibir();
            if (protocol.canal_recibir_cerrado()) {
                break;
            }

            ClientAction msg = {id, action};
            actiones_clients.push(std::move(msg));
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error receiving action from client " << id << ": " << e.what() << "\n";
            break;
        }
    }
}
