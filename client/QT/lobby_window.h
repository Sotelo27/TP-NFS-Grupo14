#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include "../../common/dto/server_msg.h"
#include "../connection/server_handler.h"

class LobbyWindow : public QWidget {
    Q_OBJECT

private:
    ServerHandler& server_handler;
    bool usuario_entro_a_sala;
    QScrollArea* scrollArea;
    QWidget* container;
    QVBoxLayout* layout;

public:
    explicit LobbyWindow(ServerHandler& server_handler, QWidget* parent = nullptr);

    void updateRoomList(const std::vector<RoomInfo>& rooms);
    void create_new_room();
    void open_waiting_room(uint8_t id_room);
    void run_lobby_loop();
};

#endif
