#ifndef SELECTION_MAP_SCREEN_H
#define SELECTION_MAP_SCREEN_H
#include <QWidget>

#include "../connection/server_handler.h"


class SelectionMapScreen : public QWidget {
    Q_OBJECT
    public:
    explicit SelectionMapScreen(ServerHandler& server_handler, QWidget* parent);

    private:
    ServerHandler& server_handler;
};

#endif //SELECTION_MAP_SCREEN_H