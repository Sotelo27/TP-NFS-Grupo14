#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <QWidget>
#include <QPushButton>
#include "../connection/server_handler.h"

class MenuScreen : public QWidget {
    Q_OBJECT
public:
    explicit MenuScreen(ServerHandler& server_handler, QWidget* parent = nullptr);

signals:
    void go_to_lobby_screen();
    void go_to_selection_car_screen();

private:
    ServerHandler& server_handler;
    QPushButton* jugarButton;
    QPushButton* seleccionarAutoButton;
};

#endif // MENU_SCREEN_H
