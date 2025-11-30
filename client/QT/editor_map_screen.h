#ifndef EDITOR_MAP_SCREEN_H
#define EDITOR_MAP_SCREEN_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>

#include "../connection/server_handler.h"

class EditorMapScreen : public QWidget {
    Q_OBJECT

public:
    explicit EditorMapScreen(ServerHandler& server_handler, QWidget* parent = nullptr);

    QString get_map_selected() const;
    QString get_file_selected() const;

    signals:
        void go_back_to_menu();
    void go_to_waiting_room();

public slots:
    void onRoomCreated(uint8_t room_id);

private:
    // === ORDEN CORRECTO ===
    ServerHandler& server_handler;

    QLabel* background;

    QPushButton* loadButton;    // 1
    QPushButton* backButton;    // 2

    QScrollArea* scrollArea;    // 3
    QListWidget* mapList;       // 4

    QWidget* container;         // 5
    QVBoxLayout* containerLayout; // 6

    QVBoxLayout* mainLayout;    // 7

    QString directory;
    QString map_selected;
    QString file_selected;

    uint8_t current_room_id;
    bool in_room;

    // === métodos privados ===
    void setupUi();
    void setupStyles();
    void setupConnections();

    void load_maps_from_directory(const QString& path);

    void onLoadClicked();

protected:
    void resizeEvent(QResizeEvent* event) override;
};

#endif // EDITOR_MAP_SCREEN_H
