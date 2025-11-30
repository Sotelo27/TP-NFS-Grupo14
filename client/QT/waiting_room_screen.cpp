#include "waiting_room_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

WaitingRoomScreen::WaitingRoomScreen(ServerHandler& server_handler, size_t& my_id, bool& map_selected, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), my_id(my_id), map_selected(map_selected)
{
    setWindowTitle("Sala de Espera - Need For Speed");
    setMinimumSize(1100, 750);

    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 30, 0, 30);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    createBackground();
    createTitle();
    createScrollArea();
    createStartButton();
    createBackButton();

    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &WaitingRoomScreen::onPollTimer);
    pollTimer->start(80);
}

// =============================================================
// CREACIÓN DE COMPONENTES
// =============================================================
void WaitingRoomScreen::createBackground() {
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->setGeometry(0, 0, width(), height());
    background->lower(); // siempre detrás
}

void WaitingRoomScreen::createTitle() {
    QLabel* title = new QLabel("SALA DE ESPERA", this);
    title->setFixedSize(820, 110);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
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
    glowTitle->setOffset(0,0);
    glowTitle->setColor(QColor(255,0,180));
    title->setGraphicsEffect(glowTitle);

    mainLayout->addWidget(title, 0, Qt::AlignHCenter);
}

void WaitingRoomScreen::createScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollArea->setFixedSize(850, 430);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   background-color: rgba(10, 0, 30, 140);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "}"
    );

    auto* glowScroll = new QGraphicsDropShadowEffect(this);
    glowScroll->setBlurRadius(55);
    glowScroll->setOffset(0,0);
    glowScroll->setColor(QColor(255,0,180));
    scrollArea->setGraphicsEffect(glowScroll);

    container = new QWidget();
    layout = new QVBoxLayout(container);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);
    layout->setAlignment(Qt::AlignTop);

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);

    mainLayout->addWidget(scrollArea, 0, Qt::AlignHCenter);
}

void WaitingRoomScreen::createStartButton() {
    // Botón de iniciar partida
    startButton = new QPushButton("INICIAR PARTIDA", this);
    startButton->setFixedSize(350, 80);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setVisible(false); // Se mostrará solo si eres admin
    startButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 0, 180, 0.25);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   color: #ff66ff;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 0, 180, 0.40); }"
        "QPushButton:pressed { background-color: rgba(255, 0, 180, 0.18); }"
    );

    auto* glowStart = new QGraphicsDropShadowEffect(this);
    glowStart->setBlurRadius(45);
    glowStart->setOffset(0,0);
    glowStart->setColor(QColor(255,0,180));
    startButton->setGraphicsEffect(glowStart);

    // Conexión con verificación de mapa
    connect(startButton, &QPushButton::clicked, this, [this]() {
        if (selected_map.isEmpty()) {
            QMessageBox::warning(this, "Mapa no seleccionado", "No elegiste mapa.");
            return;
        }
        server_handler.send_start_game({{selected_map.toStdString(), 0}});
    });

    mainLayout->addWidget(startButton, 0, Qt::AlignHCenter);

    // Botón para seleccionar mapa (otro botón distinto)
    QPushButton* selectMapButton = new QPushButton("SELECCIONAR MAPA", this);
    selectMapButton->setFixedSize(350, 80);
    selectMapButton->setCursor(Qt::PointingHandCursor);
    selectMapButton->setVisible(true);
    selectMapButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 0, 180, 0.25);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   color: #ff66ff;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 0, 180, 0.40); }"
        "QPushButton:pressed { background-color: rgba(255, 0, 180, 0.18); }"
    );

    auto* glowSelect = new QGraphicsDropShadowEffect(this);
    glowSelect->setBlurRadius(45);
    glowSelect->setOffset(0,0);
    glowSelect->setColor(QColor(255,0,180));
    selectMapButton->setGraphicsEffect(glowSelect);

    connect(selectMapButton, &QPushButton::clicked, this, [this]() {
        emit go_to_selection_map_screen();
    });

    mainLayout->addWidget(selectMapButton, 0, Qt::AlignHCenter);
}

void WaitingRoomScreen::createBackButton() {
    backButton = new QPushButton("VOLVER AL LOBBY", this);
    backButton->setFixedSize(280, 60);
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 100, 100, 0.60);"
        "   border: 3px solid rgba(200, 50, 50, 0.80);"
        "   border-radius: 12px;"
        "   font-size: 22px;"
        "   font-weight: bold;"
        "   color: white;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 120, 120, 0.75); }"
        "QPushButton:pressed { background-color: rgba(255, 80, 80, 0.55); }"
    );

    connect(backButton, &QPushButton::clicked, this, [this]() {
        server_handler.send_leave_room();
        stopPolling();
        map_selected = false;
        selected_map.clear();
        emit go_back_to_lobby_screen();
    });

    mainLayout->addWidget(backButton, 0, Qt::AlignHCenter);
}

// =============================================================
// POLLING
// =============================================================
void WaitingRoomScreen::onPollTimer() {
    for (int i = 0; i < 10; ++i) {
        ServerMessage msg = server_handler.recv_response_from_server();
        if (msg.type == ServerMessage::Type::Unknown || msg.type == ServerMessage::Type::Empty)
            break;
        processServerMessage(msg);
    }
}

// =============================================================
// PROCESAR MENSAJES DEL SERVIDOR
// =============================================================
void WaitingRoomScreen::processServerMessage(const ServerMessage& msg) {
    switch (msg.type) {
        case ServerMessage::Type::YourId:
            my_id = msg.id;
            break;

        case ServerMessage::Type::PlayersList: {
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                if (item->widget()) item->widget()->deleteLater();
                delete item;
            }

            is_admin = false;
            for (const auto& p : msg.players)
                if (p.player_id == my_id && p.is_admin)
                    is_admin = true;

            startButton->setVisible(is_admin);

            for (const auto& p : msg.players) {
                QWidget* card = new QWidget();
                card->setFixedHeight(65);
                card->setStyleSheet(
                    "QWidget {"
                    "   background-color: rgba(20,0,40,160);"
                    "   border: 3px solid #ff33cc;"
                    "   border-radius: 10px;"
                    "}"
                );

                auto* glowCard = new QGraphicsDropShadowEffect(card);
                glowCard->setBlurRadius(25);
                glowCard->setOffset(0, 0);
                glowCard->setColor(QColor(255,0,180));
                card->setGraphicsEffect(glowCard);

                QHBoxLayout* row = new QHBoxLayout(card);
                row->setContentsMargins(12, 5, 12, 5);
                row->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

                QString text = QString::fromStdString(p.username);
                if (p.is_admin) text += "  ⭐";

                QLabel* name = new QLabel(text);
                name->setStyleSheet(
                    "font-size: 22px;"
                    "font-weight: bold;"
                    "color: #ff66ff;"
                );
                row->addWidget(name);

                layout->addWidget(card);
            }

            layout->addStretch();
            break;
        }

        case ServerMessage::Type::RaceStart:
            stopPolling();
            emit go_to_game_start();
            break;

        default:
            break;
    }
}

void WaitingRoomScreen::start_game() {
    QMessageBox::information(this, "Juego iniciado", "¡La carrera está por comenzar!");
}

// =============================================================
// RESIZE EVENT PARA AJUSTAR FONDO
// =============================================================
void WaitingRoomScreen::resizeEvent(QResizeEvent* event) {
    if (background)
        background->setGeometry(0, 0, width(), height());
    QWidget::resizeEvent(event);
}
