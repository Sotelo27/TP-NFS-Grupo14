#include "protocol_client.h"

#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

#include <arpa/inet.h>

ProtocolClient::ProtocolClient(Socket&& skt): skt(std::move(skt)) {
    this->data_client.action = "";
    this->data_client.parametros = {};
}

void ProtocolClient::decodificar_action_server(uint8_t action, std::string& salida) {
    salida = "error";
    if (action == CODE_SERVER_MSG) {
        salida = SERVER_MSG;
    } else if (action == CODE_NITRO_ACTIVATED) {
        salida = SERVER_NITRO_ACTIVATED;
    } else if (action == CODE_NITRO_EXPIRED) {
        salida = SERVER_NITRO_EXPIRED;
    }
}

const DataClient& ProtocolClient::recibir() {
    uint8_t codigo = ERROR_MESSAGE;
    skt.recvall(&codigo, sizeof(codigo));
    decodificar_action_server(codigo, this->data_client.action);

    this->data_client.parametros.clear();

    int16_t cantidad_nitro_activo = 0;
    skt.recvall(&cantidad_nitro_activo, sizeof(cantidad_nitro_activo));
    cantidad_nitro_activo = ntohs(cantidad_nitro_activo);

    uint8_t situacion = ERROR_MESSAGE;
    skt.recvall(&situacion, sizeof(situacion));

    std::string situacion_str;
    decodificar_action_server(situacion, situacion_str);
    this->data_client.parametros.push_back(situacion_str);

    return this->data_client;
}

void ProtocolClient::client_nitro() {
    uint8_t codigo = CODE_CLIENT_NITRO;
    skt.sendall(&codigo, sizeof(codigo));
}
