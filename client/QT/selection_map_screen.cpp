#include "selection_map_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>

SelectionMapScreen::SelectionMapScreen(bool& map_selected, QWidget* parent)
    : QWidget(parent), map_selected(map_selected)
{
    setWindowTitle("Seleccionar Mapa - Need For Speed");
    setupUi();
}

void SelectionMapScreen::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(18);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    createBackground();

    // Título
    QLabel* title = new QLabel("SELECCIONAR MAPA", this);
    title->setFixedHeight(100);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "background-color: rgba(255, 0, 180, 0.20);"
        "border: 5px solid #ff33cc;"
        "border-radius: 18px;"
        "font-size: 48px;"
        "font-weight: 900;"
        "color: #ffccff;"
    );
    auto* titleGlow = new QGraphicsDropShadowEffect(this);
    titleGlow->setBlurRadius(55);
    titleGlow->setOffset(0, 0);
    titleGlow->setColor(QColor(255, 0, 180, 200));
    title->setGraphicsEffect(titleGlow);
    mainLayout->addWidget(title, 0, Qt::AlignHCenter);

    // Panel central
    QWidget* panel = new QWidget(this);
    panel->setStyleSheet(
        "background: rgba(15, 5, 25, 0.85);"
        "border-radius: 20px;"
        "border: 3px solid rgba(255, 0, 180, 0.18);"
    );
    auto* panelGlow = new QGraphicsDropShadowEffect(panel);
    panelGlow->setBlurRadius(60);
    panelGlow->setOffset(0, 0);
    panelGlow->setColor(QColor(255, 0, 180, 120));
    panel->setGraphicsEffect(panelGlow);

    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(36, 26, 36, 26);
    panelLayout->setSpacing(22);
    panelLayout->setAlignment(Qt::AlignTop);

    // Contenedor horizontal de maps
    QHBoxLayout* mapsLayout = new QHBoxLayout();
    mapsLayout->setSpacing(50);
    mapsLayout->setContentsMargins(8, 8, 8, 8);
    mapsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    mapsLayout->addWidget(createMapCard("assets/cities/LibertyCity.png", "Liberty City", "LibertyCity"));
    mapsLayout->addWidget(createMapCard("assets/cities/SanAndreas.png", "San Andreas", "SanAndreas"));
    mapsLayout->addWidget(createMapCard("assets/cities/ViceCity.png", "Vice City", "ViceCity"));

    panelLayout->addLayout(mapsLayout);
    mainLayout->addWidget(panel, 0, Qt::AlignHCenter);
    mainLayout->addStretch();
}

QWidget* SelectionMapScreen::createMapCard(const QString& imgPath, const QString& mapLabel, const QString& internalName) {
    QWidget* card = new QWidget(this);
    card->setFixedSize(350, 410);
    card->setStyleSheet(
        "background: rgba(255,255,255,0.04);"
        "border-radius: 16px;"
        "border: 2px solid rgba(200,120,255,0.20);"
    );
    auto* glow = new QGraphicsDropShadowEffect(card);
    glow->setBlurRadius(32);
    glow->setOffset(0, 0);
    glow->setColor(QColor(180, 100, 255, 120));
    card->setGraphicsEffect(glow);

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QLabel* imgLabel = new QLabel(card);
    QPixmap pm(imgPath);
    imgLabel->setPixmap(pm.scaled(300, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imgLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(imgLabel, 0, Qt::AlignCenter);

    QLabel* nameLabel = new QLabel(mapLabel, card);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setFixedHeight(34);
    nameLabel->setStyleSheet(
        "background: rgba(0,0,0,0.55);"
        "color: #ffdfff;"
        "font-weight: 800;"
        "font-size: 18px;"
        "border-radius: 6px;"
        "border: 1px solid rgba(150,100,255,0.35);"
    );
    layout->addWidget(nameLabel);

    QPushButton* btn = new QPushButton("Elegir", card);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedSize(120, 40);
    btn->setStyleSheet(
        "QPushButton {"
        "font-size: 16px; font-weight: 800;"
        "color: #1A1A22;"
        "background: linear-gradient(135deg, #FF9FD9, #B27CE8);"
        "border-radius: 10px;"
        "border: 2px solid rgba(178,124,232,0.85);"
        "}"
        "QPushButton:hover {"
        "background: linear-gradient(135deg, #FFB3E4, #8AF5F0);"
        "}"
        "QPushButton:pressed { background: rgba(178,124,232,0.6); }"
    );

    connect(btn, &QPushButton::clicked, this, [this, internalName]() {
        on_map_selected(internalName);
    });

    layout->addWidget(btn, 0, Qt::AlignCenter);

    return card;
}

void SelectionMapScreen::on_map_selected(const QString& map_name) {
    selected_map = map_name;
    // server_handler.send_start_game({{selected_map.toStdString(), 0}});
    map_selected = true;
    emit go_to_waiting_room_screen();
}

void SelectionMapScreen::createBackground() {
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->lower();
}

void SelectionMapScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background) background->setGeometry(0, 0, width(), height());
}
