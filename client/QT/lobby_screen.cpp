#include "lobby_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QGraphicsDropShadowEffect>

LobbyScreen::LobbyScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), my_id(my_id), waitingRoom(nullptr), in_room(false)
{
    setWindowTitle("Lobby - Need For Speed");
    setupUi();
    setupConnections();
}

void LobbyScreen::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 30, 0, 30);
    mainLayout->setSpacing(20);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    createBackground();
    createTitle();
    createButtons();
    createScrollArea();

    pollTimer = new QTimer(this);
    connect(pollTimer, &QTimer::timeout, this, &LobbyScreen::onPollTimer);
}

void LobbyScreen::createBackground() {
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->lower();
}

void LobbyScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background)
        background->setGeometry(0, 0, width(), height());
}

void LobbyScreen::createTitle() {
    QLabel* titleLabel = new QLabel("SALAS DISPONIBLES", this);
    titleLabel->setFixedSize(820, 110);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
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
    glowTitle->setOffset(0, 0);
    glowTitle->setColor(QColor(255, 0, 180));
    titleLabel->setGraphicsEffect(glowTitle);

    mainLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
}

void LobbyScreen::createButtons() {
    QPushButton* createButton = new QPushButton("CREAR NUEVA SALA", this);
    createButton->setFixedSize(420, 90);
    createButton->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 0, 180, 0.25);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 12px;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   color: #ff66ff;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 0, 180, 0.40); }"
        "QPushButton:pressed { background-color: rgba(255, 0, 180, 0.18); }"
    );

    auto* glowButton = new QGraphicsDropShadowEffect(this);
    glowButton->setBlurRadius(45);
    glowButton->setOffset(0, 0);
    glowButton->setColor(QColor(255, 0, 180));
    createButton->setGraphicsEffect(glowButton);

    connect(createButton, &QPushButton::clicked, this, &LobbyScreen::create_new_room);
    mainLayout->addWidget(createButton, 0, Qt::AlignHCenter);

    QPushButton* createButtonEditor = new QPushButton("CREAR SALA CON MAPA EDITADO", this);
    createButtonEditor->setFixedSize(600, 90);
    createButtonEditor->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(255, 0, 180, 0.25);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 12px;"
        "   font-size: 32px;"
        "   font-weight: bold;"
        "   color: #ff66ff;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover { background-color: rgba(255, 0, 180, 0.40); }"
        "QPushButton:pressed { background-color: rgba(255, 0, 180, 0.18); }"
    );

    auto* glowButtonEditor = new QGraphicsDropShadowEffect(this);
    glowButtonEditor->setBlurRadius(45);
    glowButtonEditor->setOffset(0, 0);
    glowButtonEditor->setColor(QColor(255, 0, 180));
    createButtonEditor->setGraphicsEffect(glowButtonEditor);

    connect(createButtonEditor, &QPushButton::clicked, this, [this]() {
        emit go_to_editor_screen();
    });
    mainLayout->addWidget(createButtonEditor, 0, Qt::AlignHCenter);
}

void LobbyScreen::createScrollArea() {
    scrollArea = new QScrollArea(this);
    scrollArea->setFixedSize(850, 430);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   background-color: rgba(10, 0, 30, 140);"
        "   border: 4px solid #ff33cc;"
        "   border-radius: 14px;"
        "}"
    );

    container = new QWidget();
    layout = new QVBoxLayout(container);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(12);

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);

    // Label vacío inicial
    QLabel* emptyLabel = new QLabel("No hay salas disponibles. ¡Crea una!", this);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet(
        "font-size: 26px;"
        "font-weight: bold;"
        "color: #ff66ff;"
        "padding: 18px;"
    );
    layout->addWidget(emptyLabel);

    mainLayout->addWidget(scrollArea, 0, Qt::AlignHCenter);
}

void LobbyScreen::setupConnections() {
    connect(this, &LobbyScreen::room_created, this, [this](uint8_t room_id){
        qDebug() << "APAGANDO EL POLLING DE LOBBY --- ENTRANDO A MI SALA";
        open_waiting_room(room_id);
    });
}


void LobbyScreen::onPollTimer() {
    if (in_room) return;
    for (int i = 0; i < 10; ++i) {
        ServerMessage msg = server_handler.recv_response_from_server();
        if (msg.type == ServerMessage::Type::Unknown || msg.type == ServerMessage::Type::Empty) {
            break;
        }

        if (processServerMessage(msg)) {
            qDebug() << "ME DIO VERDADEROOOOO";
            break;
        }
    }
}

bool LobbyScreen::processServerMessage(const ServerMessage& msg) {
    switch (msg.type) {
        case ServerMessage::Type::Rooms:
            if (!in_room) update_room_list(msg.rooms);
            return false;
        case ServerMessage::Type::RoomCreated:
            current_room_id = static_cast<uint8_t>(msg.id);
            in_room = true;
            qDebug() << "Sala recibida, entrando";
            emit room_created(current_room_id);
            return true;
        // case ServerMessage::Type::YourId:
        // case ServerMessage::Type::PlayersList:
        // case ServerMessage::Type::RaceStart:
        //     in_room = true;
        //     return true;
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
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(
            "font-size: 26px;"
            "font-weight: bold;"
            "color: #ff66ff;"
            "padding: 20px;"
        );
        layout->addWidget(emptyLabel);
    } else {
        for (const auto& room : rooms) {
            QWidget* row = new QWidget();
            row->setFixedHeight(90);
            row->setStyleSheet(
                "QWidget {"
                "   background-color: rgba(20, 0, 40, 160);"
                "   border: 3px solid #ff33cc;"
                "   border-radius: 12px;"
                "}"
                "QWidget:hover { background-color: rgba(255, 0, 150, 80); }"
            );

            QHBoxLayout* rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(15, 10, 15, 10);

            QString info = QString("Sala #%1   |   Jugadores: %2 / %3")
                               .arg(room.id)
                               .arg(room.current_players)
                               .arg(room.max_players);

            QLabel* roomInfo = new QLabel(info);
            roomInfo->setStyleSheet(
                "font-size: 24px;"
                "color: #ff66ff;"
                "font-weight: bold;"
            );
            rowLayout->addWidget(roomInfo);

            QPushButton* enterButton = new QPushButton("INGRESAR");
            enterButton->setFixedSize(180, 55);
            enterButton->setStyleSheet(
                "QPushButton {"
                "   background-color: rgba(255, 0, 180, 0.25);"
                "   border: 3px solid #ff33cc;"
                "   border-radius: 10px;"
                "   font-size: 22px;"
                "   font-weight: bold;"
                "   color: #ff66ff;"
                "}"
                "QPushButton:hover { background-color: rgba(255, 0, 180, 0.35); }"
                "QPushButton:pressed { background-color: rgba(255, 0, 180, 0.18); }"
                "QPushButton:disabled { background-color: rgba(80, 0, 40, 0.4); color: rgba(255,150,255,0.35); }"
            );
            enterButton->setEnabled(room.current_players < room.max_players);
            connect(enterButton, &QPushButton::clicked, [this, room_id = room.id]() {
                server_handler.send_join_room(room_id);
                qDebug() << "APAGANDO EL POLLING DE LOBBY --- ENTRANDO A SALA DE OTRO";
                open_waiting_room(room_id);
            });

            rowLayout->addWidget(enterButton);
            layout->addWidget(row, 0, Qt::AlignHCenter);
        }
    }

    layout->addStretch();
}

void LobbyScreen::create_new_room() const {
    qDebug() << "Mandando a crear la sala de espera";
    server_handler.send_create_room();
}

void LobbyScreen::open_waiting_room(uint8_t id_room) {
    if (this->pollTimer->isActive()) {
        this->pollTimer->stop();
    }
    in_room = true;
    std::cout << "[LobbyWindow] Entrando a sala " << (int)id_room << std::endl;
    emit go_to_waiting_room_screen();
}

void LobbyScreen::on_return_from_waiting_room() {
    in_room = false;
    startPolling();
}

void LobbyScreen::startPolling() {
    qDebug() << "PRENDIENDO EL POLLIING EN EL LOBBY";
    pollTimer->start(50);
}