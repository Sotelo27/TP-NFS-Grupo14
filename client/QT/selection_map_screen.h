#ifndef SELECTION_MAP_SCREEN_H
#define SELECTION_MAP_SCREEN_H
#include <QWidget>

class SelectionMapScreen : public QWidget {
    Q_OBJECT
public:
    explicit SelectionMapScreen(QWidget* parent = nullptr);

    QString get_selected_map() const { return selected_map; }

signals:
    void go_to_waiting_room_screen();

private slots:
    void on_map_selected(const QString& map_name);

private:
    QString selected_map;
};

#endif // SELECTION_MAP_SCREEN_H

