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
        "font-size: 26px; font-weight: 900; letter-spacing: 1px;"
        "color: #FF00C8; padding: 8px 6px;"
        "border-bottom: 2px solid rgba(255,0,200,0.35);"
    );
    mainLayout->addWidget(titleLabel);

    // Botón crear sala (estilo vaporwave)
    QPushButton* createButton = new QPushButton("CREAR NUEVA SALA");
    createButton->setCursor(Qt::PointingHandCursor);
    createButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 18px; font-weight: 800; letter-spacing: 1px;"
        "  color: #0afff7;"
        "  padding: 10px 18px;"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7300FF, stop:1 #FF00C8);"
        "  border: 3px solid rgba(255,255,255,0.35);"
        "  border-radius: 12px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF00C8, stop:1 #00FFE2);"
        "}"
        "QPushButton:pressed {"
        "  background:#280040;"
        "}"
    );
    connect(createButton, &QPushButton::clicked, this, &LobbyScreen::create_new_room);
    mainLayout->addWidget(createButton, 0, Qt::AlignLeft);

    // Área de scroll para salas (tono oscuro sin imagen)
    scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("QScrollArea { background-color: rgba(10,0,25,0.60); border: none; }");
    container = new QWidget();
    layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);
    container->setLayout(layout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    QLabel* emptyLabel = new QLabel("No hay salas disponibles. ¡Crea una!", this);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet(
        "font-size: 14px; font-weight: 700;"
        "color: #FF00C8; padding: 16px;"
    );
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
        emptyLabel->setStyleSheet(
            "font-size: 14px; font-weight: 700;"
            "color: #FF00C8; padding: 16px;"
        );
        layout->addWidget(emptyLabel);
    } else {
        for (const auto& room : rooms) {
            QWidget* row = new QWidget();
            row->setStyleSheet(
                "QWidget {"
                "  background-color: rgba(20,0,45,0.70);"
                "  padding: 10px; margin: 5px; border: 2px solid #7D00FF;"
                "  border-radius: 10px;"
                "}"
                "QWidget:hover {"
                "  border-color: #FF00C8;"
                "}"
            );
            QHBoxLayout* rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(8, 6, 8, 6);

            QString info = QString("Sala #%1 - Jugadores: %2/%3")
                               .arg(room.id)
                               .arg(room.current_players)
                               .arg(room.max_players);

            QLabel* roomInfo = new QLabel(info);
            roomInfo->setStyleSheet(
                "font-size: 14px; font-weight: 800; color: #E3E3FF;"
            );
            rowLayout->addWidget(roomInfo);

            QPushButton* enterButton = new QPushButton("INGRESAR");
            enterButton->setCursor(Qt::PointingHandCursor);
            enterButton->setStyleSheet(
                "QPushButton {"
                "  font-size: 14px; font-weight: 800; letter-spacing: 1px;"
                "  color: #0afff7;"
                "  padding: 6px 14px;"
                "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7300FF, stop:1 #FF00C8);"
                "  border: 2px solid rgba(255,255,255,0.35);"
                "  border-radius: 10px;"
                "}"
                "QPushButton:hover {"
                "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF00C8, stop:1 #00FFE2);"
                "}"
                "QPushButton:pressed {"
                "  background:#280040;"
                "}"
                "QPushButton:disabled {"
                "  color: rgba(200,200,200,0.4);"
                "  background: rgba(100,100,100,0.25);"
                "  border: 2px solid rgba(255,255,255,0.15);"
                "}"
            );
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

