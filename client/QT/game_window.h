#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <QDialog>
#include <QStackedWidget>
#include "lobby_screen.h"
#include "login_screen.h"
#include "result_finish_screen.h"
#include "selection_car_screen.h"
#include "selection_map_screen.h"
#include "menu_screen.h"
#include <QtMultimedia/QSound>

class GameWindow : public QDialog {
    Q_OBJECT
public:
    explicit GameWindow(ServerHandler& server_handler, size_t& my_id, bool login, QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    ServerHandler& server_handler;
    size_t& my_id;
    QStackedWidget *stack;
    QSound* sound = nullptr;

    LoginScreen *login_screen;
    LobbyScreen *lobby_screen;
    WaitingRoomScreen *waiting_room_screen;
    SelectionCarScreen *selection_car_screen;
    SelectionMapScreen *selection_map_screen;
    ResultFinishScreen *result_finish_screen;
    MenuScreen *menu_screen;

    class StartScreen; // forward declaration
    StartScreen* start_screen;

private slots:
    void go_to_lobby() const;
    void go_to_waiting_room() const;
    void go_to_car_selection() const;
    void go_to_map_selection() const;
    void go_to_results() const;
    void go_to_login() const;
    void go_to_waiting_room_from_map() const;
    void go_to_menu() const;
    void show_final_results(const std::vector<PlayerResultTotal>& results);
};

#endif //GAME_WINDOW_H
