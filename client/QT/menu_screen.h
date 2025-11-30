#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "../connection/server_handler.h"

class MenuScreen : public QWidget {
    Q_OBJECT
public:
    explicit MenuScreen(ServerHandler& server_handler, QWidget* parent = nullptr);

    int getSelectedCarIndex() const { return selectedCarIndex; }
    void setSelectedCarIndex(int idx) { selectedCarIndex = idx; }

    signals:
        void go_to_lobby_screen();
    void go_to_selection_car_screen();

private:
    ServerHandler& server_handler;
    QLabel* background;
    QPushButton* jugarButton;
    QPushButton* seleccionarAutoButton;
    int selectedCarIndex = 0;

    void createBackground();
    void createButtons();
    void setupLayout();
    void setupConnections();
};

#endif // MENU_SCREEN_H
