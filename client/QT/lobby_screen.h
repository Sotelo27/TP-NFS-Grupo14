#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "../../common/dto/server_msg.h"
#include "../connection/server_handler.h"
#include "waiting_room_screen.h"

class LobbyScreen : public QWidget {
    Q_OBJECT

public:
    explicit LobbyScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent = nullptr);
    void startPolling() { pollTimer->start(50); }
    void on_return_from_waiting_room();

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

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QLabel* background;
    QVBoxLayout* mainLayout;
    QScrollArea* scrollArea;
    QWidget* container;
    QVBoxLayout* layout;
    QTimer* pollTimer;

    uint8_t current_room_id;
    WaitingRoomScreen* waitingRoom;
    bool in_room{false};

    void setupUi();
    void setupConnections();
    void setupStyles();

    void createBackground();
    void createTitle();
    void createButtons();
    void createScrollArea();
};

#endif // LOBBY_WINDOW_H
