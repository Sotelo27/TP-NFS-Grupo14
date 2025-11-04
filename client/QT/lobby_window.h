#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include "../../common/dto/server_msg.h"
#include "../connection/server_handler.h"

class LobbyWindow : public QWidget {
    Q_OBJECT

private:
    ServerHandler& server_handler;
    size_t& my_id;
    bool usuario_entro_a_sala;
    QScrollArea* scrollArea;
    QWidget* container;
    QVBoxLayout* layout;
    QTimer* pollTimer;
    
    uint8_t current_room_id{0};
    uint32_t my_player_id{0};

    void processServerMessage(const ServerMessage& msg);

private slots:
    void onPollTimer();

public:
    explicit LobbyWindow(ServerHandler& server_handler, size_t& my_id, QWidget* parent = nullptr);

    void updateRoomList(const std::vector<RoomInfo>& rooms);
    void create_new_room();
    void open_waiting_room(uint8_t id_room);
};

#endif
