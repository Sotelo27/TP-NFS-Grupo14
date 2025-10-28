#include <exception>
#include <fstream>
#include <iostream>

#include "../common/constants.h"
#include <QApplication>
#include "QT/login_window.h"

#include "client.h"

#define HOST_NAME argv[1]
#define SERVICENAME argv[2]
#define NUMBER_ARG 3

/*
./client <hostname o IP> <servicename o puerto>

Ejemplo ./client 127.0.0.1 8080
*/
int main(int argc, char* argv[]) {
    try {
        if (argc != NUMBER_ARG) {
            throw std::invalid_argument("Bad program call. Expected " + std::string(argv[0]) +
                                        " <hostname o IP> <servicename o puerto>");
        }

        QApplication app(argc, argv);
        LoginWindow login(HOST_NAME, SERVICENAME);
        login.show();
        return app.exec();

    } catch (const std::exception& err) {
        std::cerr << "Something went wrong and an exception was caught: " << err.what() << "\n";
        return ERROR;
    } catch (...) {
        std::cerr << "Something went wrong and an unknown exception was caught.\n";
        return ERROR;
    }
}
