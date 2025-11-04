// #ifndef WAITING_ROOM_WINDOW_H
// #define WAITING_ROOM_WINDOW_H
//
// #include <QWidget>
// #include <QVBoxLayout>
// #include <QScrollArea>
// #include <QLabel>
// #include "../../common/dto/server_msg.h"
// #include "../connection/server_handler.h"
//
// class WaitingRoomWindow : public QWidget {
//     Q_OBJECT
//
// private:
//     ServerHandler& server_handler;
//
//     QScrollArea* scrollArea;
//     QWidget* container;
//     QVBoxLayout* layout;
//
//     bool salir_de_sala = false;
//
// public:
//     explicit WaitingRoomWindow(ServerHandler& server_handler, QWidget* parent = nullptr);
//
//     // Actualiza la lista completa de jugadores
//     void update_player_list(const std::vector<std::string>& players);
//
//     // Ejecuta el loop de escucha de mensajes
//     void run_waiting_loop();
// };
//
// #endif
