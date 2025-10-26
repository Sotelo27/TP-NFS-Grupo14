#include "client_thread_recv.h"

#include <utility>

ClientThreadRecv::ClientThreadRecv(ServerProtocol& protocol, size_t id,
                 Queue<ClientAction>& actiones_clients):
    protocol(protocol), id(id), actiones_clients(actiones_clients) {}

void ClientThreadRecv::run() {
    while (should_keep_running()) {
        try {
            ClientMessage received = protocol.receive();
            if (protocol.is_recv_closed()) {
                break;
            }

            if (received.type == ClientMessage::Type::Move) {
                ClientAction msg = {id, static_cast<uint8_t>(received.movement)};
                actiones_clients.push(std::move(msg));
            } else {
                // para name o unknown no hacemos nada, por ahora
            }
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error receiving action from client " << id << ": " << e.what() << "\n";
            break;
        }
    }
}
