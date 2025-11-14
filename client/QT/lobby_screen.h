#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QScrollArea>
#include <QVBoxLayout>
#include <vector>
#include "../../common/dto/server_msg.h"
#include "../connection/server_handler.h"
#include "waiting_room_screen.h"

class LobbyScreen : public QWidget {
    Q_OBJECT

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QVBoxLayout* mainLayout;
    QScrollArea* scrollArea;
    QWidget* container;
    QVBoxLayout* layout;
    QTimer* pollTimer;

    uint8_t current_room_id;

    WaitingRoomScreen* waitingRoom;

    bool in_room{false};

public:
    explicit LobbyScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent = nullptr);
    void startPolling() { pollTimer->start(50); }

private slots:
    void onPollTimer();
    void create_new_room() const;
    void open_waiting_room(uint8_t id_room);
    bool processServerMessage(const ServerMessage& msg);
    void update_room_list(const std::vector<RoomInfo>& rooms);

signals:
    void room_created(uint8_t room_id);
    void go_to_waiting_room_screen();
    void go_to_selection_map_screen();
};

#endif

