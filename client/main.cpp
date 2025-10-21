#include <exception>
#include <fstream>
#include <iostream>

#include "../common/constants.h"

#include "client.h"
#include "protocol_client.h"

#define HOST_NAME argv[1]
#define SERVICIO argv[2]
#define CANTIDAD_ARG 3

/*
./client <hostname o IP> <servicename o puerto>

Ejemplo ./client 127.0.0.1 8080
*/
int main(int argc, char* argv[]) {
    try {
        if (argc != CANTIDAD_ARG) {
            throw std::invalid_argument("Bad program call. Expected " + std::string(argv[0]) +
                                        " <hostname o IP> <servicename o puerto>");
        }

        Socket skt(HOST_NAME, SERVICIO);

        Client client(std::move(skt));

        client.procesar_actiones();

        return EXITO;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return ERROR;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return ERROR;
    }
}
