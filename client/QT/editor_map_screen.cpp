#include "editor_map_screen.h"

#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QMessageBox>
#include <yaml-cpp/yaml.h>
#include <QDebug>

// ============================================================
//  Constructor
// ============================================================

EditorMapScreen::EditorMapScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler)
{
    setupUi();
    setupStyles();
    setupConnections();

    startPolling();
}

// ============================================================
//  Polling Control
// ============================================================

void EditorMapScreen::startPolling() {
    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &EditorMapScreen::onPollTimer);
    pollTimer->start(50);
}

void EditorMapScreen::stopPolling() {
    if (pollTimer && pollTimer->isActive()) {
        pollTimer->stop();
    }
}

// ============================================================
//  Setup UI
// ============================================================

void EditorMapScreen::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 30, 0, 30);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Background
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png")
        .scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
    );
    background->setGeometry(0, 0, width(), height());
    background->lower();

    // Title
    QLabel* titleLabel = new QLabel("MAPAS CREADOS", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(110);
    titleLabel->setStyleSheet(
        "background-color: rgba(255, 0, 180, 0.20);"
        "border: 5px solid #ff33cc;"
        "border-radius: 18px;"
        "font-size: 60px;"
        "font-weight: bold;"
        "color: #ff66ff;"
        "padding: 15px;"
    );
    auto* glowTitle = new QGraphicsDropShadowEffect(this);
    glowTitle->setBlurRadius(55);
    glowTitle->setOffset(0, 0);
    glowTitle->setColor(QColor(255, 0, 180));
    titleLabel->setGraphicsEffect(glowTitle);

    mainLayout->addWidget(titleLabel);

    // Scroll area
    scrollArea = new QScrollArea(this);
    scrollArea->setFixedSize(850, 430);

    container = new QWidget();
    containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(14, 14, 14, 14);
    containerLayout->setSpacing(12);

    // Map list
    mapList = new QListWidget(container);
    containerLayout->addWidget(mapList);

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    // Load button
    loadButton = new QPushButton("CARGAR");
    loadButton->setFixedSize(420, 90);
    mainLayout->addWidget(loadButton);

    // Back to lobby button
    backButton = new QPushButton("VOLVER AL LOBBY");
    backButton->setFixedSize(350, 80);
    mainLayout->addWidget(backButton);
}

// ============================================================
//  Styling
// ============================================================

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

    // Glow & styles
    auto* glowLoad = new QGraphicsDropShadowEffect(this);
    glowLoad->setBlurRadius(45);
    glowLoad->setOffset(0, 0);
    glowLoad->setColor(QColor(255, 0, 180));
    loadButton->setGraphicsEffect(glowLoad);

    loadButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 0, 180, 0.25);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   color: #ff66ff;"
        "}"
    );

    auto* glowBack = new QGraphicsDropShadowEffect(this);
    glowBack->setBlurRadius(45);
    glowBack->setOffset(0, 0);
    glowBack->setColor(QColor(255, 0, 180));
    backButton->setGraphicsEffect(glowBack);

    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 100, 100, 0.60);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "   font-size: 28px;"
        "   color: white;"
        "}"
    );
}

// ============================================================
//  Connections
// ============================================================

void EditorMapScreen::setupConnections() {
    connect(backButton, &QPushButton::clicked, this, &EditorMapScreen::go_back_to_lobby);
    connect(loadButton, &QPushButton::clicked, this, &EditorMapScreen::onLoadClicked);
    connect(this, &EditorMapScreen::room_created, this, &EditorMapScreen::handleRoomCreated);
}

// ============================================================
//  Load Map Button
// ============================================================

void EditorMapScreen::onLoadClicked() {
    QListWidgetItem* item = mapList->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Atención", "Debes seleccionar un archivo");
        return;
    }

    file_selected = item->text() + ".yaml";

    YAML::Node config = YAML::LoadFile(directory.toStdString() + file_selected.toStdString());
    map_selected = QString::fromStdString(config["idMap"].as<std::string>());

    qDebug() << "Seleccionado archivo:" << file_selected;
    qDebug() << "Mapa seleccionado:" << map_selected;

    server_handler.send_create_room();
}

// ============================================================
//  Directory Load
// ============================================================

void EditorMapScreen::load_maps_from_directory(const QString& path) {
    directory = path;
    mapList->clear();

    QDir dir(directory);
    QStringList files = dir.entryList(QStringList() << "*.yaml", QDir::Files);

    for (const QString& file : files) {
        QFileInfo fi(file);
        mapList->addItem(fi.baseName());
    }
}

// ============================================================
//  Polling logic
// ============================================================

void EditorMapScreen::onPollTimer() {
    for (int i = 0; i < 10; ++i) {
        ServerMessage msg = server_handler.recv_response_from_server();

        if (msg.type == ServerMessage::Type::Unknown ||
            msg.type == ServerMessage::Type::Empty)
            break;

        if (processServerMessage(msg))
            break;
    }
}

bool EditorMapScreen::processServerMessage(const ServerMessage& msg) {
    switch (msg.type) {
        case ServerMessage::Type::RoomCreated:
            current_room_id = static_cast<uint8_t>(msg.id);
            emit room_created(current_room_id);
            return true;

        case ServerMessage::Type::RaceStart:
            return true;

        default:
            return false;
    }
}

// ============================================================
//  Wait Room Logic
// ============================================================

void EditorMapScreen::handleRoomCreated(uint8_t id_room) {
    stopPolling();

    in_room = true;
    qDebug() << "Entrando a la sala" << id_room;

    emit go_to_waiting_room_screen();
}

// ============================================================
//  Getters
// ============================================================

QString EditorMapScreen::get_map_selected() const {
    return map_selected;
}

QString EditorMapScreen::get_file_selected() const {
    return file_selected;
}
