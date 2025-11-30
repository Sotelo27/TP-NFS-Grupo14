#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QDialog>
#include <QStackedWidget>
#include <QtMultimedia/QSound>

#include "start_screen.h"
#include "lobby_screen.h"
#include "login_screen.h"
#include "result_finish_screen.h"
#include "selection_car_screen.h"
#include "selection_map_screen.h"
#include "menu_screen.h"
#include "editor_map_screen.h"
#include <QtMultimedia/QSound>

class GameWindow : public QDialog {
    Q_OBJECT
public:
    explicit GameWindow(ServerHandler& server_handler, size_t& my_id, bool& map_selected, bool login, QWidget *parent = nullptr);
    ~GameWindow();

private:
    void setupScreens();
    void setupLayout();
    void setupConnections();
    void setupSound();

    ServerHandler& server_handler;
    size_t& my_id;
    bool& map_selected;
    QStackedWidget* stack;
    QSound* sound;
    QLabel* background;

    StartScreen* start_screen;
    LoginScreen* login_screen;
    LobbyScreen* lobby_screen;
    WaitingRoomScreen* waiting_room_screen;
    SelectionCarScreen* selection_car_screen;
    SelectionMapScreen* selection_map_screen;
    ResultFinishScreen* result_finish_screen;
    MenuScreen* menu_screen;
    EditorMapScreen* editor_map_screen;

private slots:
    void goToLobby();
    void goToWaitingRoom();
    void goToCarSelection();
    void goToMapSelection();
    void goToResults();
    void goToLogin();
    void goToWaitingRoomFromMap();
    void goToMenu();
};

#endif // GAME_WINDOW_H
