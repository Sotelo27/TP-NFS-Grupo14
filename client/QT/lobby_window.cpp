#include "lobby_window.h"

#include <iostream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include "../client_game.h"

LobbyWindow::LobbyWindow(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), usuario_entro_a_sala(false)
{
    setWindowTitle("Lobby - Need For Speed");
    setFixedSize(600, 400);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Título
    QLabel* titleLabel = new QLabel("Salas Disponibles", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(titleLabel);

    // Botón crear sala
    QPushButton* createButton = new QPushButton("Crear nueva sala");
    createButton->setStyleSheet("font-size: 16px; padding: 10px; background-color: #2E8B57; color: white;");
    connect(createButton, &QPushButton::clicked, this, &LobbyWindow::create_new_room);
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
    
    // Mensaje inicial si no hay salas
    QLabel* emptyLabel = new QLabel("No hay salas disponibles. ¡Crea una!", this);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet("font-size: 14px; color: gray; padding: 20px;");
    layout->addWidget(emptyLabel);

    setLayout(mainLayout);

    // Configurar timer para polling de mensajes del servidor
    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &LobbyWindow::onPollTimer);
    pollTimer->start(50); // Polling cada 50ms para mejor responsividad
    
    std::cout << "[LobbyWindow] Ventana creada, timer de polling iniciado" << std::endl;
}

void LobbyWindow::onPollTimer() {
    if (usuario_entro_a_sala) {
        pollTimer->stop();
        std::cout << "[LobbyWindow] Usuario en sala, deteniendo polling" << std::endl;
        return;
    }
    
    // Procesar múltiples mensajes en cada tick
    int messages_processed = 0;
    for (int i = 0; i < 10; ++i) { // Procesar hasta 10 mensajes por tick
        ServerMessage msg = server_handler.recv_response_from_server();
        
        if (msg.type == ServerMessage::Type::Unknown) {
            break; // No hay más mensajes
        }
        
        messages_processed++;
        processServerMessage(msg);
    }
    
    if (messages_processed > 0) {
        std::cout << "[LobbyWindow] Processed " << messages_processed << " messages" << std::endl;
    }
}

void LobbyWindow::processServerMessage(const ServerMessage& msg) {
    if (msg.type == ServerMessage::Type::Rooms) {
        std::cout << "[LobbyWindow] Recibida lista de salas: " << msg.rooms.size() << std::endl;
        updateRoomList(msg.rooms);
    } else if (msg.type == ServerMessage::Type::RoomCreated) {
        std::cout << "[LobbyWindow] Sala creada con ID: " << (int)msg.id << std::endl;
        current_room_id = static_cast<uint8_t>(msg.id);
    } else if (msg.type == ServerMessage::Type::YourId) {
        std::cout << "[LobbyWindow] Tu ID asignado: " << msg.id << std::endl;
        my_player_id = msg.id;
        
        // Detener polling
        usuario_entro_a_sala = true;
        pollTimer->stop();
        
        std::cout << "[LobbyWindow] Iniciando juego real con SDL..." << std::endl;
        
        // Cerrar lobby
        this->close();
        
        // Iniciar el juego REAL con SDL - Usar el auto verde (CommonGreenCar)
        try {
            ClientGame game(CarSpriteID::CommonGreenCar, my_player_id, server_handler);
            game.start();
        } catch (const std::exception& e) {
            std::cerr << "[LobbyWindow] Error iniciando el juego: " << e.what() << std::endl;
            QMessageBox::critical(nullptr, "Error", 
                QString("No se pudo iniciar el juego: %1").arg(e.what()));
        }
        
    } else if (msg.type == ServerMessage::Type::PlayerName) {
        std::cout << "[LobbyWindow] Nombre de jugador recibido: " << msg.username << " (id: " << msg.id << ")" << std::endl;
    } else if (msg.type == ServerMessage::Type::Pos) {
        std::cout << "[LobbyWindow] Ignorando mensaje POS (id=" << msg.id << ", x=" << msg.x << ", y=" << msg.y << ")" << std::endl;
    }
}

void LobbyWindow::updateRoomList(const std::vector<RoomInfo>& rooms) {
    std::cout << "[LobbyWindow] Actualizando UI con " << rooms.size() << " salas..." << std::endl;
    
    // Limpiar lista actual
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
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
    std::cout << "[LobbyWindow] UI actualizada" << std::endl;
}

void LobbyWindow::create_new_room() {
    std::cout << "[LobbyWindow] Solicitando creación de sala..." << std::endl;
    server_handler.send_create_room();
    
    // Mostrar feedback visual
    QMessageBox::information(this, "Creando sala", "Esperando confirmación del servidor...");
}

void LobbyWindow::open_waiting_room(uint8_t id_room) {
    usuario_entro_a_sala = true;
    pollTimer->stop();
    
    std::cout << "[LobbyWindow] Entrando a sala " << (int)id_room << std::endl;
    
    // Ya no cerrar la ventana aquí, esperar a recibir YOUR_ID
    // this->close();
}