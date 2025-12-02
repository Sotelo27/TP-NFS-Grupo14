#ifndef SELECTION_MAP_SCREEN_H
#define SELECTION_MAP_SCREEN_H

#include <QWidget>
#include <QString>
#include <QLabel>          // <--- agregar esto
#include <QVBoxLayout>
#include "../connection/server_handler.h"

class SelectionMapScreen : public QWidget {
    Q_OBJECT
public:
    explicit SelectionMapScreen(QWidget* parent = nullptr);

    QString get_selected_map() const { return selected_map; }

signals:
    void go_to_waiting_room_screen();

private slots:
    void on_map_selected(const QString& map_name);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUi();
    void createBackground();
    QWidget* createMapCard(const QString& imgPath, const QString& mapLabel, const QString& internalName);

    QString selected_map;

    QVBoxLayout* mainLayout;
    QLabel* background;   // ya está declarado correctamente
};

#endif // SELECTION_MAP_SCREEN_H
