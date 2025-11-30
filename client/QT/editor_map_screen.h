// #ifndef EDITOR_MAP_SCREEN_H
// #define EDITOR_MAP_SCREEN_H
//
// #include <QWidget>
// #include <QScrollArea>
// #include <QVBoxLayout>
// #include <QPushButton>
// #include <QLabel>
// #include <QDir>
// #include <QListWidget>
//
// #include "qt_handler.h"
//
// class EditorMapScreen : public QWidget {
//     Q_OBJECT
//
// public:
//     explicit EditorMapScreen(QTHandler& qt_handler, QWidget* parent = nullptr);
//
//     void load_maps_from_directory(const QString& path);
//     QString get_map_selected() const;
//     QString get_file_selected() const;
//
//     signals:
//         void go_back_to_lobby();
//     void go_to_waiting_room_screen();
//
// private slots:
//     void onLoadClicked();
//     void onRoomCreated(uint8_t room_id);
//
// private:
//     // --- CORE ---
//     QTHandler& qt_handler;
//
//     // --- MAP STATE ---
//     QString directory;
//     QString file_selected;
//     QString map_selected;
//     uint8_t current_room_id;
//     bool in_room{false};
//
//     // --- UI ---
//     QLabel* background;
//     QVBoxLayout* mainLayout;
//     QScrollArea* scrollArea;
//     QWidget* container;
//     QVBoxLayout* containerLayout;
//     QListWidget* mapList;
//     QPushButton* loadButton;
//     QPushButton* backButton;
//
//     // --- SETUP ---
//     void setupUi();
//     void setupStyles();
//     void setupConnections();
// };
//
// #endif // EDITOR_MAP_SCREEN_H
