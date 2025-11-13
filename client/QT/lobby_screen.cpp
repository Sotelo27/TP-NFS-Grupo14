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
    mainLayout->setContentsMargins(16, 12, 16, 12); 
    mainLayout->setSpacing(10);

    // Título
    QLabel* titleLabel = new QLabel("Salas Disponibles", this);
    titleLabel->setStyleSheet(
        "font-size: 24px; font-weight: 700;"
        "color: #B27CE8; padding: 8px;"
        "background-color: rgba(255,255,255,0.35);"
        "border: 1px solid rgba(178,124,232,0.4); border-radius: 10px;"
    );
    mainLayout->addWidget(titleLabel);

    // Botón crear sala 
    QPushButton* createButton = new QPushButton("Crear nueva sala");
    createButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 16px; font-weight: 600;"
        "  color: #3B3B44;"
        "  padding: 8px 18px;"
        "  background: linear-gradient(135deg, rgba(255,159,217,0.55), rgba(178,124,232,0.55));"
        "  border: 2px solid rgba(178,124,232,0.45);"
        "  border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "  background: linear-gradient(135deg, rgba(255,159,217,0.70), rgba(120,230,224,0.65));"
        "}"
        "QPushButton:pressed { background: rgba(178,124,232,0.55); }"
    );
    connect(createButton, &QPushButton::clicked, this, &LobbyScreen::create_new_room);
    mainLayout->addWidget(createButton, 0, Qt::AlignLeft);

    // Área de scroll para salas (tono oscuro sin imagen)
    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet(
        "QScrollArea { background-color: rgba(240,240,245,0.55); border: 1px solid rgba(178,124,232,0.35); border-radius: 12px; }"
    );
    container = new QWidget();
    layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);
    container->setLayout(layout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    QLabel* emptyLabel = new QLabel("No hay salas disponibles. ¡Crea una!", this);
    emptyLabel->setStyleSheet(
        "font-size: 14px; font-weight: 600;"
        "color: #B27CE8; padding: 14px;"
    );
    layout->addWidget(emptyLabel);

    setLayout(mainLayout);

    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &LobbyScreen::onPollTimer);
    pollTimer->start(50);

    connect(this, &LobbyScreen::room_created, this, [this](uint8_t room_id) {
        std::cout << "[LobbyWindow] Sala creada con ID: " << (int)room_id << "\n";
        open_waiting_room(room_id);
    });

    std::cout << "[LobbyWindow] Ventana creada, polling iniciado" << std::endl;
}

void LobbyScreen::onPollTimer() {
    for (int i = 0; i < 10; ++i) {
        ServerMessage msg = server_handler.recv_response_from_server();
        if (msg.type == ServerMessage::Type::Unknown || msg.type == ServerMessage::Type::Empty)
            break;
        // Si processServerMessage devuelve true, cortar inmediatamente para no drenar YOUR_ID/PLAYERS_LIST
        if (processServerMessage(msg)) {
            break;
        }
    }
}

// Cambiar a bool: true => cortar loop actual (se navega a waiting room)
bool LobbyScreen::processServerMessage(const ServerMessage& msg) {
    switch (msg.type) {
        case ServerMessage::Type::Rooms:
            update_room_list(msg.rooms);
            return false;

        case ServerMessage::Type::RoomCreated:
            std::cout << "[LobbyWindow] Sala creada con ID: " << (int)msg.id << std::endl;
            current_room_id = static_cast<uint8_t>(msg.id);
            emit room_created(current_room_id);
            // Se navega en el slot -> detener el loop actual para no consumir más mensajes
            return true;

        default:
            return false;
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
        emptyLabel->setStyleSheet(
            "font-size: 14px; font-weight: 600;"
            "color: #B27CE8; padding: 14px;"
        );
        layout->addWidget(emptyLabel);
    } else {
        for (const auto& room : rooms) {
            QWidget* row = new QWidget();
            row->setStyleSheet(
                "QWidget {"
                "  background-color: rgba(255,255,255,0.70);"
                "  padding: 8px; margin: 4px; border: 1px solid rgba(178,124,232,0.45);"
                "  border-radius: 10px;"
                "}"
                "QWidget:hover { border-color: rgba(255,159,217,0.65); }"
            );
            QHBoxLayout* rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(8, 6, 8, 6);

            QString info = QString("Sala #%1 - Jugadores: %2/%3")
                               .arg(room.id)
                               .arg(room.current_players)
                               .arg(room.max_players);

            QLabel* roomInfo = new QLabel(info);
            roomInfo->setStyleSheet(
                "font-size: 13px; font-weight: 600; color: #3B3B44;"
            );
            rowLayout->addWidget(roomInfo);

            QPushButton* enterButton = new QPushButton("Ingresar");
            enterButton->setStyleSheet(
                "QPushButton {"
                "  font-size: 13px; font-weight: 600;"
                "  color: #3B3B44;"
                "  padding: 6px 14px;"
                "  background: linear-gradient(135deg, rgba(255,159,217,0.50), rgba(178,124,232,0.50));"
                "  border: 2px solid rgba(178,124,232,0.40);"
                "  border-radius: 8px;"
                "}"
                "QPushButton:hover {"
                "  background: linear-gradient(135deg, rgba(255,159,217,0.65), rgba(120,230,224,0.60));"
                "}"
                "QPushButton:pressed { background: rgba(178,124,232,0.50); }"
                "QPushButton:disabled {"
                "  color: rgba(90,90,90,0.4);"
                "  background: rgba(200,200,205,0.35);"
                "  border: 2px solid rgba(178,124,232,0.20);"
                "}"
            );
            enterButton->setEnabled(room.current_players < room.max_players);
            rowLayout->addWidget(enterButton);

            connect(enterButton, &QPushButton::clicked, [this, room_id = room.id]() {
                std::cout << "[LobbyWindow] Usuario solicita unirse a sala " << (int)room_id << std::endl;
                // Importante: detener polling antes de transicionar y de que lleguen YOUR_ID/PLAYERS_LIST
                if (pollTimer && pollTimer->isActive()) pollTimer->stop();
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
    
    // ELIMINADO: No hacer nada más aquí. El slot de room_created se encargará de todo
}

void LobbyScreen::open_waiting_room(uint8_t id_room) {
    if (pollTimer->isActive())
        pollTimer->stop();

    std::cout << "[LobbyWindow] Entrando a sala " << (int)id_room << std::endl;

    emit go_to_waiting_room_screen();
}


