#include "lobby_screen.h"

#include <iostream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

LobbyScreen::LobbyScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      my_id(my_id),
      waitingRoom(nullptr)
{
    setWindowTitle("Lobby - Need For Speed");
    setFixedSize(800, 600);

    mainLayout = new QVBoxLayout(this);

    // Título
    QLabel* titleLabel = new QLabel("Salas Disponibles", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    // Botón crear sala
    QPushButton* createButton = new QPushButton("Crear nueva sala");
    createButton->setStyleSheet("font-size: 16px; padding: 10px; background-color: #2E8B57; color: black;");
    connect(createButton, &QPushButton::clicked, this, &LobbyScreen::create_new_room);
    mainLayout->addWidget(createButton);

    // Área de scroll para salas
    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("background-color: #F0F0F0;");
    container = new QWidget();
    layout = new QVBoxLayout(container);
    container->setLayout(layout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    QLabel* emptyLabel = new QLabel("No hay salas disponibles. ¡Crea una!", this);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet("font-size: 14px; color: gray; padding: 20px;");
    layout->addWidget(emptyLabel);

    setLayout(mainLayout);

    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &LobbyScreen::onPollTimer);
    pollTimer->start(50);

    connect(this, &LobbyScreen::room_created, this, [this](uint8_t room_id) {
        QMessageBox::information(this, "Sala creada",
                                 "¡La sala fue creada correctamente!");
        open_waiting_room(room_id);
    });

    std::cout << "[LobbyWindow] Ventana creada, polling iniciado" << std::endl;
}

void LobbyScreen::onPollTimer() {
    for (int i = 0; i < 10; ++i) {
        ServerMessage msg = server_handler.recv_response_from_server();
        if (msg.type == ServerMessage::Type::Unknown)
            break;
        processServerMessage(msg);
    }
}

void LobbyScreen::processServerMessage(const ServerMessage& msg) {
    switch (msg.type) {
        case ServerMessage::Type::Rooms:
            update_room_list(msg.rooms);
            break;

        case ServerMessage::Type::RoomCreated:
            std::cout << "[LobbyWindow] Sala creada con ID: " << (int)msg.id << std::endl;
            current_room_id = static_cast<uint8_t>(msg.id);
            emit room_created(current_room_id);
            break;

        default:
            break;
    }
}

void LobbyScreen::update_room_list(const std::vector<RoomInfo>& rooms) {
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }

    if (rooms.empty()) {
        QLabel* emptyLabel = new QLabel("No hay salas disponibles. ¡Crea una!");
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("font-size: 14px; color: gray; padding: 20px;");
        layout->addWidget(emptyLabel);
    } else {
        for (const auto& room : rooms) {
            QWidget* row = new QWidget();
            row->setStyleSheet("background-color: white; padding: 10px; margin: 5px; border: 1px solid #ccc;");
            QHBoxLayout* rowLayout = new QHBoxLayout(row);

            QString info = QString("Sala #%1 - Jugadores: %2/%3")
                               .arg(room.id)
                               .arg(room.current_players)
                               .arg(room.max_players);

            QLabel* roomInfo = new QLabel(info);
            roomInfo->setStyleSheet("font-size: 14px; font-weight: bold;");
            rowLayout->addWidget(roomInfo);

            QPushButton* enterButton = new QPushButton("Ingresar");
            enterButton->setStyleSheet("background-color: #4CAF50; color: white; padding: 5px 15px;");
            enterButton->setEnabled(room.current_players < room.max_players);
            rowLayout->addWidget(enterButton);

            connect(enterButton, &QPushButton::clicked, [this, room_id = room.id]() {
                std::cout << "[LobbyWindow] Usuario solicita unirse a sala " << (int)room_id << std::endl;
                server_handler.send_join_room(room_id);
                open_waiting_room(room_id);
            });

            layout->addWidget(row);
        }
    }

    layout->addStretch();
}

void LobbyScreen::create_new_room() const {
    std::cout << "[LobbyWindow] Solicitando creación de sala..." << std::endl;
    server_handler.send_create_room();

    QTimer::singleShot(0, const_cast<LobbyScreen*>(this), [this]() {
        const_cast<LobbyScreen*>(this)->open_waiting_room(0);
    });
}

void LobbyScreen::open_waiting_room(uint8_t id_room) {
    if (pollTimer->isActive())
        pollTimer->stop();

    std::cout << "[LobbyWindow] Entrando a sala " << (int)id_room << std::endl;

    emit go_to_waiting_room_screen();
}

