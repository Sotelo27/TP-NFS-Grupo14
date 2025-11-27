#ifndef LOBBY_WINDOW_H
#define LOBBY_WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>

#include "../../common/dto/server_msg.h"
#include "../connection/server_handler.h"
#include "waiting_room_screen.h"

class LobbyScreen : public QWidget {
    Q_OBJECT

public:
    explicit LobbyScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent = nullptr);

    void startPolling();
    void on_return_from_waiting_room();

protected:
    void resizeEvent(QResizeEvent* event) override;

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
    void go_to_maps_created_from_editor_screen();

private:

    // --- Inicialización ---
    void setupBackground();
    void setupMainLayout();
    void setupTitle();
    void setupCreateRoomButton();
    void setupEditorMapButton();
    void setupScrollArea();
    void setupEmptyLabel();
    void setupPolling();
    void setupRoomCreatedConnection();

    // --- Referencias externas ---
    ServerHandler& server_handler;
    size_t& my_id;

    // --- UI ---
    QLabel* background{nullptr};
    QVBoxLayout* mainLayout{nullptr};
    QScrollArea* scrollArea{nullptr};
    QWidget* container{nullptr};
    QVBoxLayout* layout{nullptr};

    // --- Timers ---
    QTimer* pollTimer{nullptr};

    // --- Estado ---
    uint8_t current_room_id{0};
    WaitingRoomScreen* waitingRoom{nullptr};
    bool in_room{false};
};

#endif // LOBBY_WINDOW_H
