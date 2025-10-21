#include "client_thread_send.h"

ClientThreadSend::ClientThreadSend(ProtocolServer& protocol, size_t id,
                                 Queue<server_msg>& mensajes_a_enviar):
        protocol(protocol), id(id), mensajes_a_enviar(mensajes_a_enviar) {}

void ClientThreadSend::run() {
    while (should_keep_running()) {
        try {
            server_msg msg = mensajes_a_enviar.pop();
            protocol.enviar_mensaje(msg.cantidad_nitros_activos, msg.mensaje);
        } catch (const std::exception& e) {
            if (!should_keep_running()) {
                break;
            }

            std::cerr << "Error sending message to client " << id << ": " << e.what() << "\n";
            break;
        }
    }
}
