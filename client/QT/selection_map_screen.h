#ifndef SELECTION_MAP_SCREEN_H
#define SELECTION_MAP_SCREEN_H
#include <QWidget>

#include "../connection/server_handler.h"


class SelectionMapScreen : public QWidget {
    Q_OBJECT
public:
    explicit SelectionMapScreen(ServerHandler& server_handler, QWidget* parent);

    QString get_selected_map() const { return selected_map; }

signals:
    void go_to_waiting_room_screen();

private slots:
    void on_map_selected(const QString& map_name);

private:
    ServerHandler& server_handler;
    QString selected_map; // NUEVO
};

#endif //SELECTION_MAP_SCREEN_H