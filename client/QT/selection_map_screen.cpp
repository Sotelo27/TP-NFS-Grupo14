#include "selection_map_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPalette>

SelectionMapScreen::SelectionMapScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    // tamaño y título de ventana (coherente con otras pantallas)
    setWindowTitle("Seleccionar Mapa - Need For Speed");
    setFixedSize(1100, 750);

    // ----------------------------
    // Fondo: intento robusto usando BOTH palette y stylesheet
    // ----------------------------
    // Palette (como en Lobby)
    QPalette pal;
    pal.setBrush(QPalette::Window, QPixmap("assets/images/fondo.png"));
    setAutoFillBackground(true);
    setPalette(pal);

    // También stylesheet por si el palette no se aplica en alguna plataforma
    // (no quita la imagen, solo asegura fallback visual)
    setStyleSheet(
        "SelectionMapScreen { background-image: url('assets/images/fondo.png');"
        " background-repeat: no-repeat; background-position: center; background-size: cover; }"
    );

    // ----------------------------
    // Layout principal
    // ----------------------------
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(18);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // ----------------------------
    // Cartel título (neón)
    // ----------------------------
    QLabel* title = new QLabel("SELECCIONAR MAPA", this);
    title->setFixedSize(980, 100);
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

    // ----------------------------
    // Panel central opaco (evita que el fondo interfiera)
    // ----------------------------
    QWidget* panel = new QWidget(this);
    panel->setFixedSize(1020, 560);
    panel->setStyleSheet(
        "background: rgba(15, 5, 25, 0.85);"    // panel oscuro semi-opa
        "border-radius: 20px;"
        "border: 3px solid rgba(255, 0, 180, 0.18);"
    );
    auto* panelGlow = new QGraphicsDropShadowEffect(this);
    panelGlow->setBlurRadius(60);
    panelGlow->setOffset(0, 0);
    panelGlow->setColor(QColor(255, 0, 180, 120));
    panel->setGraphicsEffect(panelGlow);

    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(36, 26, 36, 26);
    panelLayout->setSpacing(22);
    panelLayout->setAlignment(Qt::AlignTop);

    // -------------------------------------------------
    // Contenedor horizontal de las 3 cards
    // -------------------------------------------------
    QHBoxLayout* mapsLayout = new QHBoxLayout();
    mapsLayout->setSpacing(50);
    mapsLayout->setContentsMargins(8, 8, 8, 8);
    mapsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // Helper para crear cards
    auto createMapCard = [&](const QString& imgPath,
                             const QString& mapLabel,
                             const QString& internalName)
    {
        // caja externa para card (con borde y sombra)
        QWidget* cardWidget = new QWidget(panel);
        cardWidget->setFixedSize(300, 360);
        cardWidget->setStyleSheet(
            "background: rgba(255,255,255,0.04);"
            "border-radius: 16px;"
            "border: 2px solid rgba(200,120,255,0.20);"
        );
        auto* cardGlow = new QGraphicsDropShadowEffect(cardWidget);
        cardGlow->setBlurRadius(32);
        cardGlow->setOffset(0, 0);
        cardGlow->setColor(QColor(180, 100, 255, 120));
        cardWidget->setGraphicsEffect(cardGlow);

        QVBoxLayout* cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(12, 12, 12, 12);
        cardLayout->setSpacing(10);
        cardLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

        // caja para la imagen (con fondo oscuro para aislar del ladrillo)
        QWidget* imgBox = new QWidget(cardWidget);
        imgBox->setFixedSize(276, 190);
        imgBox->setStyleSheet(
            "background: rgba(10,0,20,0.55);"
            "border-radius: 12px;"
            "border: 1px solid rgba(180,120,255,0.25);"
        );
        QVBoxLayout* imgBoxLayout = new QVBoxLayout(imgBox);
        imgBoxLayout->setContentsMargins(8, 8, 8, 8);

        QLabel* imgLabel = new QLabel(imgBox);
        QPixmap pm(imgPath);
        imgLabel->setPixmap(pm.scaled(220, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLabel->setAlignment(Qt::AlignCenter);
        imgBoxLayout->addWidget(imgLabel, 0, Qt::AlignCenter);

        // nombre del mapa (barra)
        QLabel* nameLabel = new QLabel(mapLabel, cardWidget);
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

        // botón elegir
        QPushButton* chooseBtn = new QPushButton("Elegir", cardWidget);
        chooseBtn->setCursor(Qt::PointingHandCursor);
        chooseBtn->setFixedSize(120, 40);
        chooseBtn->setStyleSheet(
            "QPushButton {"
            "  font-size: 16px; font-weight: 800;"
            "  color: #1A1A22;"
            "  background: linear-gradient(135deg, #FF9FD9, #B27CE8);"
            "  border-radius: 10px;"
            "  border: 2px solid rgba(178,124,232,0.85);"
            "}"
            "QPushButton:hover {"
            "  background: linear-gradient(135deg, #FFB3E4, #8AF5F0);"
            "}"
            "QPushButton:pressed { background: rgba(178,124,232,0.6); }"
        );

        connect(chooseBtn, &QPushButton::clicked, this, [this, internalName]() {
            on_map_selected(internalName);
        });

        // armar card
        cardLayout->addWidget(imgBox, 0, Qt::AlignCenter);
        cardLayout->addSpacing(6);
        cardLayout->addWidget(nameLabel);
        cardLayout->addSpacing(12);
        cardLayout->addWidget(chooseBtn, 0, Qt::AlignCenter);

        return cardWidget;
    };

    // Crear y agregar las 3 cards
    QWidget* card1 = createMapCard("assets/cities/Liberty City.png",  "Liberty City",  "LibertyCity");
    QWidget* card2 = createMapCard("assets/cities/San Andreas.png",  "San Andreas",  "SanAndreas");
    QWidget* card3 = createMapCard("assets/cities/Vice City.png",    "Vice City",    "ViceCity");

    mapsLayout->addWidget(card1);
    mapsLayout->addWidget(card2);
    mapsLayout->addWidget(card3);

    // agregar mapsLayout al panel
    panelLayout->addLayout(mapsLayout);

    // agregar panel al mainLayout
    mainLayout->addWidget(panel, 0, Qt::AlignHCenter);

    // espacio abajo
    mainLayout->addStretch();
}

// ------------------------------------------------------
// Cuando se selecciona el mapa
// ------------------------------------------------------
void SelectionMapScreen::on_map_selected(const QString& map_name) {
    selected_map = map_name;

    std::string chosen = selected_map.isEmpty()
        ? "LibertyCity"
        : selected_map.toStdString();

    // Mantengo la lógica que tenías: send_start_game y volver a sala de espera
    server_handler.send_start_game({{chosen, 0}});
    emit go_to_waiting_room_screen();
}
