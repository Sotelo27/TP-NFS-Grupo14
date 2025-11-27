#include "waiting_room_screen.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <iostream>

WaitingRoomScreen::WaitingRoomScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      my_id(my_id)
{
    setWindowTitle("Sala de Espera - Need For Speed");
    setFixedSize(1100, 750);

    // =============================================================
    // FONDO NEÓN
    // =============================================================
    QPalette pal;
    pal.setBrush(QPalette::Window, QPixmap("assets/images/fondo.png"));
    setAutoFillBackground(true);
    setPalette(pal);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 30, 0, 0);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // =============================================================
    // TITULO
    // =============================================================
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

    mainLayout->addWidget(title);

    // =============================================================
    // SCROLL AREA DE JUGADORES
    // =============================================================
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

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);

    mainLayout->addWidget(scrollArea);

    // =============================================================
    // BOTÓN INICIAR PARTIDA (Sólo admin)
    // =============================================================
    startButton = new QPushButton("INICIAR PARTIDA");
    startButton->setFixedSize(350, 80);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setVisible(false);

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

    connect(startButton, &QPushButton::clicked, this, [this]() {
        emit go_to_selection_map_screen();
    });

    mainLayout->addWidget(startButton, 0, Qt::AlignCenter);

    // =============================================================
    // BOTÓN VOLVER AL LOBBY
    // =============================================================
    backButton = new QPushButton("VOLVER AL LOBBY");
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
        this->server_handler.send_leave_room();
        stopPolling();
        emit go_back_to_lobby_screen();
    });

    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);

    // =============================================================
    // POLLING
    // =============================================================
    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &WaitingRoomScreen::onPollTimer);
    pollTimer->start(80);
}

// =============================================================
// POLLING DE MENSAJES
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

            // Limpio items anteriores
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                if (item->widget()) item->widget()->deleteLater();
                delete item;
            }

            is_admin = false;
            for (const auto& p : msg.players) {
                if (p.player_id == my_id && p.is_admin)
                    is_admin = true;
            }

            startButton->setVisible(is_admin);

            // Render de jugadores
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
            emit go_to_game_start();
            break;

        default:
            break;
    }
}


void WaitingRoomScreen::start_game() {
    QMessageBox::information(this, "Juego iniciado", "¡La carrera está por comenzar!");
}


void WaitingRoomScreen::fromEditorScreen() {

}