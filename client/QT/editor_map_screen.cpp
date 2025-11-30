#include "editor_map_screen.h"

#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <yaml-cpp/yaml.h>
#include <QDebug>

EditorMapScreen::EditorMapScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      pollTimer(new QTimer(this)),        // << ORDEN CORRECTO
      background(nullptr),
      loadButton(nullptr),
      backButton(nullptr),
      scrollArea(nullptr),
      mapList(nullptr),
      container(nullptr),
      containerLayout(nullptr),
      mainLayout(nullptr),
      directory("editor/MapsEdited/"),
      map_selected(""),
      file_selected(""),
      current_room_id(0),
      in_room(false)
{
    setupUi();
    setupStyles();
    setupConnections();

    start_polling();
    load_maps_from_directory(directory);
}

void EditorMapScreen::start_polling() {
    connect(pollTimer, &QTimer::timeout, this, &EditorMapScreen::onPollTimer);
    pollTimer->start(50);
}

void EditorMapScreen::stop_polling() {
    if (pollTimer->isActive())
        pollTimer->stop();
}

void EditorMapScreen::onPollTimer() {
    for (int i = 0; i < 10; i++) {
        ServerMessage msg = server_handler.recv_response_from_server();

        if (msg.type == ServerMessage::Type::Unknown ||
            msg.type == ServerMessage::Type::Empty)
            break;

        if (in_room)
            break;

        if (msg.type == ServerMessage::Type::RoomCreated) {
            uint8_t room_id = static_cast<uint8_t>(msg.id);
            current_room_id = room_id;
            in_room = true;

            qDebug() << "[EditorMapScreen] RoomCreated id =" << (int)room_id;

            stop_polling();


            emit go_to_waiting_room(room_id);
            break;
        }
    }
}

void EditorMapScreen::setupUi() {
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->lower();

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QLabel* titleLabel = new QLabel("EDITOR DE MAPAS", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(90);
    titleLabel->setStyleSheet(
        "background-color: rgba(255, 0, 180, 0.20);"
        "border: 5px solid #ff33cc;"
        "border-radius: 18px;"
        "font-size: 48px;"
        "font-weight: bold;"
        "color: #ff66ff;"
    );

    auto* glowTitle = new QGraphicsDropShadowEffect(this);
    glowTitle->setBlurRadius(45);
    glowTitle->setOffset(0, 0);
    glowTitle->setColor(QColor(255, 0, 180));
    titleLabel->setGraphicsEffect(glowTitle);

    mainLayout->addWidget(titleLabel);

    scrollArea = new QScrollArea(this);
    scrollArea->setFixedSize(850, 430);

    container = new QWidget();
    containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(14, 14, 14, 14);
    containerLayout->setSpacing(12);

    mapList = new QListWidget(container);
    containerLayout->addWidget(mapList);

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    loadButton = new QPushButton("CARGAR MAPA");
    loadButton->setFixedSize(340, 70);
    mainLayout->addWidget(loadButton);

    backButton = new QPushButton("VOLVER");
    backButton->setFixedSize(340, 70);
    mainLayout->addWidget(backButton);
}

void EditorMapScreen::setupStyles() {
    mapList->setStyleSheet(
        "QListWidget {"
        "   background-color: rgba(20,0,40,160);"
        "   border: 3px solid #ff33cc;"
        "   border-radius: 12px;"
        "   font-size: 24px;"
        "   color: #ff66ff;"
        "   font-weight: bold;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: rgba(255,0,180,120);"
        "}"
    );

    loadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255,0,180,0.25);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "   font-size: 30px;"
        "   font-weight: bold;"
        "   color: #ff66ff;"
        "}"
    );

    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 100, 100, 0.60);"
        "   border: 3px solid #ff33cc;"
        "   border-radius: 12px;"
        "   font-size: 24px;"
        "   color: white;"
        "}"
    );
}

void EditorMapScreen::setupConnections() {
    connect(loadButton, &QPushButton::clicked, this, &EditorMapScreen::onLoadClicked);

    connect(backButton, &QPushButton::clicked, this, [this]() {
        stop_polling();
        emit go_back_to_menu();
    });
}

void EditorMapScreen::load_maps_from_directory(const QString& path) {
    mapList->clear();

    QDir dir(path);

    if (!dir.exists()) {
        QMessageBox::warning(this, "Error", "El directorio /editor/MapsEdited/ NO existe.");
        return;
    }

    QStringList files = dir.entryList(QStringList() << "*.yaml", QDir::Files);

    for (const QString& filename : files) {
        QFileInfo fi(filename);
        mapList->addItem(fi.baseName());
    }
}

void EditorMapScreen::onLoadClicked() {
    QListWidgetItem* item = mapList->currentItem();

    if (!item) {
        QMessageBox::warning(this, "Atención", "Debes seleccionar un archivo");
        return;
    }

    file_selected = item->text() + ".yaml";

    YAML::Node config = YAML::LoadFile((directory + file_selected).toStdString());
    map_selected = QString::fromStdString(config["idMap"].as<std::string>());

    qDebug() << "Archivo seleccionado:" << file_selected;
    qDebug() << "Mapa seleccionado:" << map_selected;

    server_handler.send_create_room();
}

void EditorMapScreen::onRoomCreated(uint8_t room_id) {
    current_room_id = room_id;
    in_room = true;

    qDebug() << "Sala creada con ID:" << room_id;
}

void EditorMapScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background)
        background->setGeometry(0, 0, width(), height());
}

QString EditorMapScreen::get_map_selected() const {
    return map_selected;
}

QString EditorMapScreen::get_file_selected() const {
    return file_selected;
}
