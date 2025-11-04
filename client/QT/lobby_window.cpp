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

LobbyWindow::LobbyWindow(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), usuario_entro_a_sala(false)
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
    std::cout << "[Client] Solicitando creación de sala..." << std::endl;
    server_handler.send_create_room();
}

void LobbyWindow::open_waiting_room(uint8_t id_room) {
    usuario_entro_a_sala = true;
    std::cout << "[Client] Entrando a sala " << (int)id_room << std::endl;
    
    // Enviar JOIN-ROOM para salas existentes (no para la que acabamos de crear)
    server_handler.send_join_room(id_room);
    
    QMessageBox::information(this, "Sala", 
        QString("Ingresaste a la sala #%1\nEsperando que otros jugadores se unan...").arg(id_room));
}

void LobbyWindow::run_lobby_loop() {
    bool running = true;
    while (running && !usuario_entro_a_sala) {
        ServerMessage msg = server_handler.recv_response_from_server();

        if (msg.type == ServerMessage::Type::Rooms) {
            std::cout << "[Client] Recibida lista de salas: " << msg.rooms.size() << std::endl;
            updateRoomList(msg.rooms);
        } else if (msg.type == ServerMessage::Type::RoomCreated) {
            std::cout << "[Client] Sala creada con ID: " << (int)msg.id << std::endl;
            QMessageBox::information(this, "Sala creada", 
                QString("La sala #%1 fue creada correctamente.").arg(msg.id));
            open_waiting_room(static_cast<uint8_t>(msg.id));
        } else if (msg.type == ServerMessage::Type::Unknown) {
            // No hay mensajes, pequeña pausa
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        if (usuario_entro_a_sala) {
            running = false;
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        QApplication::processEvents();
    }
}