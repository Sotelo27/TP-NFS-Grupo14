#ifndef SELECTION_CAR_SCREEN_H
#define SELECTION_CAR_SCREEN_H
#include <QWidget>

#include "../connection/server_handler.h"
#include "../../common/enum/car_enum.h"

class SelectionCarScreen : public QWidget {
    Q_OBJECT
    public:
    explicit SelectionCarScreen(ServerHandler& server_handler, QWidget* parent);

    private:
    ServerHandler& server_handler;

    signals:
    void go_to_lobby();
    void car_selected(CarSpriteID id);
};


#endif //SELECTION_CAR_SCREEN_H