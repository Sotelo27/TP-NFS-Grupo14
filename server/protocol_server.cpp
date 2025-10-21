#include "protocol_server.h"

#include <cstring>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>

#include <arpa/inet.h>

#include "../common/constants.h"

ProtocolServer::ProtocolServer(Socket&& skt): skt(std::move(skt)) {}

uint8_t ProtocolServer::recibir() {
    uint8_t codigo;
    skt.recvall(&codigo, sizeof(codigo));

    return codigo;
}

void ProtocolServer::enviar_mensaje(uint16_t cantidad_nitros_activos, uint8_t mensaje) {
    uint8_t codigo = CODE_SERVER_MSG;

    cantidad_nitros_activos = htons(cantidad_nitros_activos);

    std::vector<char> paquete(sizeof(codigo) + sizeof(cantidad_nitros_activos) + sizeof(mensaje));
    size_t offset = 0;
    std::memcpy(paquete.data() + offset, &codigo, sizeof(codigo));
    offset += sizeof(codigo);
    std::memcpy(paquete.data() + offset, &cantidad_nitros_activos, sizeof(cantidad_nitros_activos));
    offset += sizeof(cantidad_nitros_activos);
    std::memcpy(paquete.data() + offset, &mensaje, sizeof(mensaje));

    skt.sendall(paquete.data(), paquete.size());
}

bool ProtocolServer::canal_recibir_cerrado() const { return skt.is_stream_recv_closed(); }

void ProtocolServer::shutdown(int modo) { skt.shutdown(modo); }
