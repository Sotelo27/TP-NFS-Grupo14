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
    title->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 10px;");
    mainLayout->addWidget(title);

    scrollArea = new QScrollArea(this);
    container = new QWidget(this);
    layout = new QVBoxLayout(container);
    container->setLayout(layout);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    QPushButton* startButton = new QPushButton("Iniciar partida");
    startButton->setStyleSheet("padding: 10px; background-color: #0078D7; color: black;");
    connect(startButton, &QPushButton::clicked, [this]() {
        std::cout << "[WaitingRoomWindow] Iniciando partida..." << std::endl;
        emit go_to_game_start();
    });
    mainLayout->addWidget(startButton);

    // 🔹 nuevo botón para volver al lobby
    QPushButton* backButton = new QPushButton("Volver al lobby");
    backButton->setStyleSheet("padding: 10px; background-color: #d9534f; color: black;");
    connect(backButton, &QPushButton::clicked, this, [this]() {
        emit go_back_to_lobby_screen(); // emite señal al LobbyWindow
    });
    mainLayout->addWidget(backButton);

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
            // Limpiamos los bloques anteriores de jugadores
            QLayoutItem* item;
            while ((item = layout->takeAt(0)) != nullptr) {
                if (item->widget()) item->widget()->deleteLater();
                delete item;
            }

            // Agregamos un QLabel para cada jugador
            for (const auto& player : msg.players) {
                QLabel* playerLabel = new QLabel(QString::fromStdString(player.username), this);
                playerLabel->setStyleSheet(
                    "background-color: #e0e0e0; "
                    "padding: 8px; "
                    "margin: 4px; "
                    "border-radius: 5px; "
                    "font-size: 16px;"
                );
                layout->addWidget(playerLabel);

                std::cout << "[WaitingRoomWindow] Jugador recibido: "
                          << player.username << " (id=" << player.player_id << ")" << std::endl;
            }

            layout->addStretch(); // empuja los bloques hacia arriba
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
