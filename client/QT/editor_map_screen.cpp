#include "editor_map_screen.h"

#include <QGraphicsDropShadowEffect>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <yaml-cpp/yaml.h>
#include <QDebug>

// ============================================================
//  Constructor
// ============================================================

EditorMapScreen::EditorMapScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      background(nullptr),
      loadButton(nullptr),
      backButton(nullptr),
      scrollArea(nullptr),
      mapList(nullptr),
      container(nullptr),
      containerLayout(nullptr),
      mainLayout(nullptr),
      in_room(false)
{
    directory = "editor/MapsEdited/";

    setupUi();
    setupStyles();
    setupConnections();

    load_maps_from_directory(directory);
}

// ============================================================
//  UI
// ============================================================

void EditorMapScreen::setupUi() {
    // Fondo FULLSCREEN
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->lower();

    // Layout principal
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 40, 50, 40);
    mainLayout->setSpacing(10);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Título
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

    // Lista de mapas
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

    // Botón cargar mapa
    loadButton = new QPushButton("CARGAR MAPA");
    loadButton->setFixedSize(340, 70);
    mainLayout->addWidget(loadButton);

    // Botón volver
    backButton = new QPushButton("VOLVER");
    backButton->setFixedSize(340, 70);
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

// ============================================================
//  Connections
// ============================================================

void EditorMapScreen::setupConnections() {
    connect(loadButton, &QPushButton::clicked, this, &EditorMapScreen::onLoadClicked);
    connect(backButton, &QPushButton::clicked, this, &EditorMapScreen::go_back_to_menu);
}

// ============================================================
//  Load Directory (AUTO)
// ============================================================

void EditorMapScreen::load_maps_from_directory(const QString& path) {
    mapList->clear();

    QDir dir(path);

    if (!dir.exists()) {
        QMessageBox::warning(this, "Error", "El directorio /editor/MapsEdited/ NO existe.");
        return;
    }

    // SOLO archivos .yaml
    QStringList files = dir.entryList(QStringList() << "*.yaml", QDir::Files);

    for (const QString& filename : files) {
        QFileInfo fi(filename);
        mapList->addItem(fi.baseName());   // SIN .yaml
    }
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

    YAML::Node config = YAML::LoadFile((directory + file_selected).toStdString());
    map_selected = QString::fromStdString(config["idMap"].as<std::string>());

    qDebug() << "Archivo seleccionado:" << file_selected;
    qDebug() << "Mapa seleccionado:" << map_selected;

    server_handler.send_create_room();
}

// ============================================================
//  Handle Room Created  (LLAMADO DESDE AFUERA)
// ============================================================

void EditorMapScreen::onRoomCreated(uint8_t room_id) {
    current_room_id = room_id;
    in_room = true;

    qDebug() << "Sala creada con ID:" << room_id;

    emit go_to_waiting_room();
}

// ============================================================
//  Resize Background
// ============================================================

void EditorMapScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background)
        background->setGeometry(0, 0, width(), height());
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
