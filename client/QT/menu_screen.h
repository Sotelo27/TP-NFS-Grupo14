#ifndef MENU_SCREEN_H
#define MENU_SCREEN_H

#include <QWidget>
#include <QPushButton>
#include "../connection/server_handler.h"

class MenuScreen : public QWidget {
    Q_OBJECT
public:
    explicit MenuScreen(QWidget* parent = nullptr);

    int getSelectedCarIndex() const { return selectedCarIndex; }
    void setSelectedCarIndex(int idx) { selectedCarIndex = idx; }

signals:
    void go_to_lobby_screen();
    void go_to_selection_car_screen();

private:
    QPushButton* jugarButton;
    QPushButton* seleccionarAutoButton;
    int selectedCarIndex = 0;
};

#endif // MENU_SCREEN_H
