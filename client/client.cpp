#include "client.h"

#include <iostream>
#include <utility>

Client::Client(Socket&& skt): protocol(std::move(skt)) {}

void Client::procesar_respuesta(const DataClient& respuesta) {
    const std::string& action = respuesta.action;
    const std::string& parametro = respuesta.parametros[0];

    if (action != SERVER_MSG) {
        throw std::runtime_error("unknown server action: " + action);
    }

    if (parametro == SERVER_NITRO_ACTIVATED) {
        std::cout << HIT_NITRO << "\n";
    } else if (parametro == SERVER_NITRO_EXPIRED) {
        std::cout << OUT_NITRO << "\n";
    } else {
        throw std::runtime_error("unknown server parameter: " + parametro);
    }
}

void Client::procesar_respuestas(int cantidad) {
    for (int i = 0; i < cantidad; ++i) {
        const DataClient& respuesta = protocol.recibir();
        procesar_respuesta(respuesta);
    }
}

void Client::procesar_action(const std::string& action,
                              const std::optional<std::string>& parametro) {
    if (action == CLIENT_ACTION_NITRO) {
        protocol.client_nitro();
    } else if (action == CLIENT_ACTION_READ) {
        procesar_respuestas(std::stoi(parametro.value()));
    } else {
        throw std::runtime_error("unknown action: " + action);
    }
}

void Client::validar_numero(const std::string& str) {
    int num;
    try {
        num = std::stoi(str);
    } catch (const std::exception&) {
        throw std::invalid_argument("'" + str + "' is not a valid number");
    }

    if (num < 1) {
        throw std::invalid_argument("'" + str + "' must be greater than 0");
    }
}

bool Client::parsear_comando(const std::string& entrada, std::string& action,
                              std::string& argumento) {
    std::istringstream iss(entrada);

    if (!(iss >> action)) {
        return false;
    }

    if (action == CLIENT_ACTION_READ) {
        if (!(iss >> argumento)) {
            throw std::invalid_argument("read: missing argument");
        }
        validar_numero(argumento);
    }

    return true;
}

void Client::procesar_actiones() {
    std::string entrada;
    while (std::getline(std::cin, entrada) && entrada != CLIENT_ACTION_EXIT) {
        try {
            std::string action, argumento;
            if (parsear_comando(entrada, action, argumento)) {
                if (action == CLIENT_ACTION_READ) {
                    procesar_action(action, argumento);
                } else {
                    procesar_action(action);
                }
            }
        } catch (const std::invalid_argument& err) {
            std::cerr << "Input error: " << err.what() << "\n";
        } catch (const std::exception& err) {
            std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        } catch (...) {
            std::cerr << "Something went wrong and an unknown exception was caught.\n";
        }
    }
}
