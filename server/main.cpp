#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

#include "../common/constants.h"

#include "server.h"

#define PUERTO argv[1]
#define CANTIDAD_ARG 2

/*
./server <servicename o puerto>

Ejemplo: ./server 8080
*/
int main(int argc, char* argv[]) {
    try {
        if (argc != CANTIDAD_ARG) {
            throw std::invalid_argument("Bad program call. Expected " + std::string(argv[0]) +
                                        " <servicename o puerto>");
        }
        Server server(PUERTO);
        server.start();
        return EXITO;
    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return ERROR;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return ERROR;
    }
}
