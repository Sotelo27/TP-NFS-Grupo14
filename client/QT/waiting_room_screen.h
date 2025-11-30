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
    bool& map_selected;

    QLabel* background = nullptr;
    QScrollArea* scrollArea = nullptr;
    QWidget* container = nullptr;
    QVBoxLayout* layout = nullptr;
    QTimer* pollTimer = nullptr;
    QVBoxLayout* mainLayout = nullptr;
    QPushButton* selectMapButton = nullptr;
    QPushButton* startButton = nullptr;
    QPushButton* backButton = nullptr;

    bool is_admin = false;
    QString selected_map;

public:
    explicit WaitingRoomScreen(ServerHandler& server_handler, size_t& my_id, bool& map_selected, QWidget* parent = nullptr);

    void start_game();
    void set_selected_map(const QString& map) { selected_map = map; }
    bool isAdmin() const { return is_admin; }
    void hideSelectMapButton();

    void startPolling() { if (pollTimer && !pollTimer->isActive()) pollTimer->start(50); }
    void stopPolling()  { if (pollTimer && pollTimer->isActive()) pollTimer->stop(); }

    signals:
    void go_back_to_lobby_screen();
    void go_to_selection_car_screen();
    void go_to_selection_map_screen();
    void go_to_game_start();
    void go_to_editor_screen();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void onPollTimer();
    void processServerMessage(const ServerMessage& msg);

    void createBackground();
    void createTitle();
    void createScrollArea();
    void createStartButton();
    void createBackButton();
};

#endif // WAITING_ROOM_WINDOW_H
