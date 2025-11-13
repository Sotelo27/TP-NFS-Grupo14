#ifndef WAITING_ROOM_WINDOW_H
#define WAITING_ROOM_WINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include "../../common/dto/server_msg.h"
#include "../connection/server_handler.h"

class WaitingRoomScreen : public QWidget {
    Q_OBJECT

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QVBoxLayout* playerListLayout;
    QScrollArea* scrollArea;
    QWidget* container;
    QVBoxLayout* layout;
    QTimer* pollTimer;

    // control de admin y botón de inicio
    QPushButton* startButton = nullptr;
    bool is_admin = false;

    QString selected_map; // NUEVO

public:
    explicit WaitingRoomScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent);
    void update_player_list(const std::vector<std::string>& players);
    void start_game();
    void set_selected_map(const QString& map) { selected_map = map; }

    void startPolling() { if (pollTimer && !pollTimer->isActive()) pollTimer->start(50); }
    void stopPolling()  { if (pollTimer &&  pollTimer->isActive()) pollTimer->stop(); }

    signals:
        void go_back_to_lobby_screen();
        void go_to_selection_car_screen();
        void go_to_selection_map_screen();
        void go_to_game_start();

private slots:
    void onPollTimer();
    void processServerMessage(const ServerMessage& msg);
};

#endif
