#include "lobby_window.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>
#include "waiting_room_window.h"

LobbyWindow::LobbyWindow(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    setWindowTitle("Lobby");
    setFixedSize(600, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QPushButton* createButton = new QPushButton("Crear nueva sala");
    connect(createButton, &QPushButton::clicked, this, &LobbyWindow::create_new_room);
    mainLayout->addWidget(createButton);

    scrollArea = new QScrollArea(this);
    container = new QWidget();
    layout = new QVBoxLayout(container);
    container->setLayout(layout);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);
    mainLayout->addWidget(scrollArea);

    setLayout(mainLayout);
}

void LobbyWindow::updateRoomList(const std::vector<RoomInfo>& rooms) {
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (auto& room : rooms) {
        QWidget* row = new QWidget();
        QHBoxLayout* rowLayout = new QHBoxLayout(row);

        QString info = QString("Sala %1 (%2/%3)")
                           .arg(room.id)
                           .arg(room.current_players)
                           .arg(room.max_players);

        QLabel* roomInfo = new QLabel(info);
        rowLayout->addWidget(roomInfo);

        QPushButton* enterButton = new QPushButton("Ingresar");
        rowLayout->addWidget(enterButton);

        connect(enterButton, &QPushButton::clicked, [this, room_id = room.id]() {
            open_waiting_room(room_id);
        });

        layout->addWidget(row);
    }

    layout->addStretch();
}

void LobbyWindow::create_new_room() {
    server_handler.send_create_room();
    ServerMessage msg = server_handler.recv_response_from_server();
    if (msg.type == ServerMessage::Type::Rooms) {
        QMessageBox::information(this, "Sala creada", "La sala fue creada correctamente.");
        open_waiting_room(msg.id);
    } else {
        QMessageBox::warning(this, "Error", "No se pudo crear la sala.");
    }
}

void LobbyWindow::open_waiting_room(uint8_t id_room) {
    usuario_entro_a_sala = true;
    std::cout << "Entrando a sala " << (int)id_room << std::endl;
    // WaitingRoomWindow waiting_room(server_handler);
    // waiting_room.show();
    this->close();
}

void LobbyWindow::run_lobby_loop() {
    bool running = true;
    while (running && !usuario_entro_a_sala) {
        ServerMessage msg = server_handler.recv_response_from_server();

        if (msg.type == ServerMessage::Type::Rooms) {
            updateRoomList(msg.rooms);
        }

        if (usuario_entro_a_sala) {
            running = false;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QApplication::processEvents();
    }
}
