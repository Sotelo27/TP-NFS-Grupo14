// #include "editor_map_screen.h"
// #include <QGraphicsDropShadowEffect>
// #include <QListWidgetItem>
// #include <QFileInfo>
// #include <QMessageBox>
// #include <yaml-cpp/yaml.h>
// #include <QDebug>
// #include <QVBoxLayout>
//
// EditorMapScreen::EditorMapScreen(QTHandler& qt_handler, QWidget* parent)
//     : QWidget(parent),
//       qt_handler(qt_handler)
// {
//     setupUi();
//     setupStyles();
//     setupConnections();
// }
//
// // ============================================================
// //  Setup UI
// // ============================================================
//
// void EditorMapScreen::setupUi() {
//     mainLayout = new QVBoxLayout(this);
//     mainLayout->setContentsMargins(0, 30, 0, 30);
//     mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
//
//     background = new QLabel(this);
//     background->setPixmap(QPixmap("assets/images/fondo.png")
//         .scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
//     background->setGeometry(0, 0, width(), height());
//     background->lower();
//
//     QLabel* titleLabel = new QLabel("MAPAS CREADOS", this);
//     titleLabel->setAlignment(Qt::AlignCenter);
//     titleLabel->setFixedHeight(110);
//     titleLabel->setStyleSheet(
//         "background-color: rgba(255, 0, 180, 0.20);"
//         "border: 5px solid #ff33cc;"
//         "border-radius: 18px;"
//         "font-size: 60px;"
//         "font-weight: bold;"
//         "color: #ff66ff;"
//         "padding: 15px;"
//     );
//     auto* glowTitle = new QGraphicsDropShadowEffect(this);
//     glowTitle->setBlurRadius(55);
//     glowTitle->setOffset(0, 0);
//     glowTitle->setColor(QColor(255, 0, 180));
//     titleLabel->setGraphicsEffect(glowTitle);
//
//     mainLayout->addWidget(titleLabel);
//
//     scrollArea = new QScrollArea(this);
//     scrollArea->setFixedSize(850, 430);
//
//     container = new QWidget();
//     containerLayout = new QVBoxLayout(container);
//     containerLayout->setContentsMargins(14, 14, 14, 14);
//     containerLayout->setSpacing(12);
//
//     mapList = new QListWidget(container);
//     containerLayout->addWidget(mapList);
//
//     scrollArea->setWidgetResizable(true);
//     scrollArea->setWidget(container);
//     mainLayout->addWidget(scrollArea);
//
//     loadButton = new QPushButton("CARGAR");
//     loadButton->setFixedSize(420, 90);
//     mainLayout->addWidget(loadButton);
//
//     backButton = new QPushButton("VOLVER AL LOBBY");
//     backButton->setFixedSize(350, 80);
//     mainLayout->addWidget(backButton);
// }
//
// // ============================================================
// //  Styling
// // ============================================================
//
// void EditorMapScreen::setupStyles() {
//     mapList->setStyleSheet(
//         "QListWidget {"
//         "   background-color: rgba(20,0,40,160);"
//         "   border: 3px solid #ff33cc;"
//         "   border-radius: 12px;"
//         "   font-size: 24px;"
//         "   color: #ff66ff;"
//         "   font-weight: bold;"
//         "}"
//         "QListWidget::item:selected {"
//         "   background-color: rgba(255,0,180,120);"
//         "}"
//     );
//
//     auto* glowLoad = new QGraphicsDropShadowEffect(this);
//     glowLoad->setBlurRadius(45);
//     glowLoad->setOffset(0, 0);
//     glowLoad->setColor(QColor(255, 0, 180));
//     loadButton->setGraphicsEffect(glowLoad);
//
//     loadButton->setStyleSheet(
//         "QPushButton {"
//         "   background-color: rgba(255, 0, 180, 0.25);"
//         "   border: 4px solid #ff33cc;"
//         "   border-radius: 14px;"
//         "   font-size: 32px;"
//         "   font-weight: bold;"
//         "   color: #ff66ff;"
//         "}"
//     );
//
//     auto* glowBack = new QGraphicsDropShadowEffect(this);
//     glowBack->setBlurRadius(45);
//     glowBack->setOffset(0, 0);
//     glowBack->setColor(QColor(255, 0, 180));
//     backButton->setGraphicsEffect(glowBack);
//
//     backButton->setStyleSheet(
//         "QPushButton {"
//         "   background-color: rgba(255, 100, 100, 0.60);"
//         "   border: 4px solid #ff33cc;"
//         "   border-radius: 14px;"
//         "   font-size: 28px;"
//         "   color: white;"
//         "}"
//     );
// }
//
// // ============================================================
// //  Connections
// // ============================================================
//
// void EditorMapScreen::setupConnections() {
//     connect(backButton, &QPushButton::clicked, this, &EditorMapScreen::go_back_to_lobby);
//
//     connect(loadButton, &QPushButton::clicked, this, &EditorMapScreen::onLoadClicked);
//
//     // 🔥 IMPORTANTE: recibir señal desde QTHandler cuando el server responde
//     connect(&qt_handler, &QTHandler::roomCreated,
//             this, &EditorMapScreen::onRoomCreated);
// }
//
// // ============================================================
// //  Load Map Button
// // ============================================================
//
// void EditorMapScreen::onLoadClicked() {
//     QListWidgetItem* item = mapList->currentItem();
//     if (!item) {
//         QMessageBox::warning(this, "Atención", "Debes seleccionar un archivo");
//         return;
//     }
//
//     file_selected = item->text() + ".yaml";
//
//     YAML::Node config = YAML::LoadFile(directory.toStdString() + file_selected.toStdString());
//     map_selected = QString::fromStdString(config["idMap"].as<std::string>());
//
//     qDebug() << "Seleccionado archivo:" << file_selected;
//     qDebug() << "Mapa seleccionado:" << map_selected;
//
//     // Ahora enviamos por QTHandler
//     qt_handler.send_create_room();
// }
//
// // ============================================================
// //  Directory Load
// // ============================================================
//
// void EditorMapScreen::load_maps_from_directory(const QString& path) {
//     directory = path;
//     mapList->clear();
//
//     QDir dir(directory);
//     QStringList files = dir.entryList(QStringList() << "*.yaml", QDir::Files);
//
//     for (const QString& file : files) {
//         QFileInfo fi(file);
//         mapList->addItem(fi.baseName());
//     }
// }
//
// // ============================================================
// //  Handle Room Created
// // ============================================================
//
// void EditorMapScreen::onRoomCreated(uint8_t room_id) {
//     current_room_id = room_id;
//     in_room = true;
//
//     qDebug() << "Sala creada, ID:" << room_id;
//
//     emit go_to_waiting_room_screen();
// }
//
// // ============================================================
// //  Getters
// // ============================================================
//
// QString EditorMapScreen::get_map_selected() const {
//     return map_selected;
// }
//
// QString EditorMapScreen::get_file_selected() const {
//     return file_selected;
// }
