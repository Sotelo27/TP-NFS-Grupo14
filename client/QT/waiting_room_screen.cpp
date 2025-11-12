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
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 26px; font-weight: 900; letter-spacing: 1px;"
        "color: #FF00C8; padding: 8px 6px;"
        "border-bottom: 2px solid rgba(255,0,200,0.35);"
    );
    mainLayout->addWidget(title);

    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet(
        "QScrollArea { background-color: rgba(10,0,25,0.60); border: none; }"
    );
    container = new QWidget(this);
    layout = new QVBoxLayout(container);
    layout->setContentsMargins(10,10,10,10);
    layout->setSpacing(8);
    container->setLayout(layout);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    QPushButton* startButton = new QPushButton("INICIAR PARTIDA");
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 18px; font-weight: 800; letter-spacing: 1px;"
        "  color: #0afff7; padding: 12px 24px;"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7300FF, stop:1 #FF00C8);"
        "  border: 3px solid rgba(255,255,255,0.35); border-radius: 14px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF00C8, stop:1 #00FFE2);"
        "}"
        "QPushButton:pressed { background:#280040; }"
    );
    connect(startButton, &QPushButton::clicked, [this]() {
        std::cout << "[WaitingRoomWindow] Iniciando partida..." << std::endl;
        emit go_to_game_start();
    });
    mainLayout->addWidget(startButton, 0, Qt::AlignCenter);

    QPushButton* backButton = new QPushButton("VOLVER AL LOBBY");
    backButton->setCursor(Qt::PointingHandCursor);
    backButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 14px; font-weight: 700;"
        "  color: #FF00C8; padding: 8px 18px;"
        "  background-color: rgba(20,0,45,0.70);"
        "  border: 2px solid #7D00FF; border-radius: 10px;"
        "}"
        "QPushButton:hover { border-color:#FF00C8; color:#00FFE2; }"
        "QPushButton:pressed { background:#280040; }"
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
                    "  font-size: 14px; font-weight: 700; color:#E3E3FF;"
                    "  background-color: rgba(20,0,45,0.70);"
                    "  padding: 8px 12px; border: 2px solid #7D00FF;"
                    "  border-radius: 10px;"
                    "}"
                    "QLabel:hover { border-color:#FF00C8; }"
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
