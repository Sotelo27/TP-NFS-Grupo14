#include "waiting_room_screen.h"
#include <QPushButton>
#include <QMessageBox>
#include <iostream>
#include "../../common/enum/car_enum.h"
#include "../../common/enum/map_enum.h"

WaitingRoomScreen::WaitingRoomScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), my_id(my_id)
{
    setMinimumSize(800, 600);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Sala de Espera", this);
    title->setStyleSheet(
        "font-size: 24px; font-weight: 700;"
        "color: #B27CE8; padding: 8px;"
        "background-color: rgba(255,255,255,0.35);"
        "border: 1px solid rgba(178,124,232,0.4); border-radius: 10px;"
    );
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet(
        "QScrollArea { background-color: rgba(240,240,245,0.55); border: 1px solid rgba(178,124,232,0.35); border-radius: 12px; }"
    );
    container = new QWidget(this);
    layout = new QVBoxLayout(container);
    layout->setContentsMargins(10,10,10,10);
    layout->setSpacing(8);
    container->setLayout(layout);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    QPushButton* startButton = new QPushButton("Iniciar partida");
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 16px; font-weight: 700;"
        "  color: #3B3B44;"
        "  padding: 10px 22px;"
        "  background: linear-gradient(135deg, rgba(255,159,217,0.55), rgba(178,124,232,0.55));"
        "  border: 2px solid rgba(178,124,232,0.45);"
        "  border-radius: 12px;"
        "}"
        "QPushButton:hover {"
        "  background: linear-gradient(135deg, rgba(255,159,217,0.70), rgba(120,230,224,0.65));"
        "}"
        "QPushButton:pressed { background: rgba(178,124,232,0.55); }"
    );
    connect(startButton, &QPushButton::clicked, [this]() {
        std::cout << "[WaitingRoomWindow] Iniciando partida..." << std::endl;
        emit go_to_game_start();
    });
    mainLayout->addWidget(startButton, 0, Qt::AlignCenter);

    QPushButton* backButton = new QPushButton("Volver al lobby");
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 14px; font-weight: 600;"
        "  color: #3B3B44;"
        "  padding: 8px 18px;"
        "  background-color: rgba(255,255,255,0.55);"
        "  border: 2px solid rgba(178,124,232,0.40);"
        "  border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255,255,255,0.75);"
        "  border-color: rgba(255,159,217,0.60);"
        "}"
        "QPushButton:pressed { background-color: rgba(178,124,232,0.40); }"
    );
    connect(backButton, &QPushButton::clicked, this, [this]() {
        emit go_back_to_lobby_screen();
    });
    mainLayout->addWidget(backButton, 0, Qt::AlignCenter);

    setLayout(mainLayout);

    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &WaitingRoomScreen::onPollTimer);
    pollTimer->start(50);

    std::cout << "[WaitingRoomWindow] Esperando mensajes del servidor..." << std::endl;
}

void WaitingRoomScreen::onPollTimer() {
    for (int i = 0; i < 10; ++i) {
        ServerMessage msg = server_handler.recv_response_from_server();

        if (msg.type == ServerMessage::Type::Unknown)
            break;

        processServerMessage(msg);
    }
}

void WaitingRoomScreen::processServerMessage(const ServerMessage& msg) {
    switch (msg.type) {
        case ServerMessage::Type::YourId:
            std::cout << "[WaitingRoomWindow] Tu ID asignado: " << msg.id << std::endl;
            my_id = msg.id;
            break;

        case ServerMessage::Type::PlayersList: {
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                if (item->widget()) item->widget()->deleteLater();
                delete item;
            }
            for (const auto& player : msg.players) {
                QLabel* playerLabel = new QLabel(QString::fromStdString(player.username), this);
                playerLabel->setStyleSheet(
                    "QLabel {"
                    "  font-size: 13px; font-weight: 600; color:#3B3B44;"
                    "  background-color: rgba(255,255,255,0.75);"
                    "  padding: 6px 10px; border: 1px solid rgba(178,124,232,0.40);"
                    "  border-radius: 8px;"
                    "}"
                    "QLabel:hover { border-color: rgba(255,159,217,0.60); }"
                );
                layout->addWidget(playerLabel);
                std::cout << "[WaitingRoomWindow] Jugador recibido: "
                          << player.username << " (id=" << player.player_id << ")" << std::endl;
            }
            layout->addStretch();
            break;
        }
        default:
            break;
    }
}

void WaitingRoomScreen::update_player_list(const std::vector<std::string>& players) {
    QLayoutItem* item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    for (const auto& name : players) {
        QLabel* label = new QLabel(QString::fromStdString(name), this);
        label->setStyleSheet("font-size: 16px;");
        layout->addWidget(label);
    }

    layout->addStretch();
}

void WaitingRoomScreen::start_game() {
    QMessageBox::information(this, "Juego iniciado", "¡La carrera está por comenzar!");
}
