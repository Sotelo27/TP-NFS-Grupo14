// #include "waiting_room_window.h"
// #include <QHBoxLayout>
// #include <QPushButton>
// #include <QMessageBox>
// #include <iostream>
//
// WaitingRoomWindow::WaitingRoomWindow(ServerHandler& server_handler, QWidget* parent)
//     : QWidget(parent), server_handler(server_handler) {
//
//     setWindowTitle("Sala de espera");
//     setFixedSize(400, 300);
//
//     QVBoxLayout* mainLayout = new QVBoxLayout(this);
//
//     scrollArea = new QScrollArea(this);
//     container = new QWidget();
//     layout = new QVBoxLayout(container);
//     container->setLayout(layout);
//     scrollArea->setWidgetResizable(true);
//     scrollArea->setWidget(container);
//     mainLayout->addWidget(scrollArea);
//
//     QPushButton* leaveButton = new QPushButton("Salir de la sala");
//     connect(leaveButton, &QPushButton::clicked, [this]() {
//         salir_de_sala = true;
//         this->close();
//     });
//     mainLayout->addWidget(leaveButton);
//
//     setLayout(mainLayout);
// }
//
// void WaitingRoomWindow::update_player_list(const std::vector<std::string>& players) {
//     QLayoutItem* child;
//     while ((child = layout->takeAt(0)) != nullptr) {
//         delete child->widget();
//         delete child;
//     }
//
//     for (const auto& name : players) {
//         QLabel* playerLabel = new QLabel(QString::fromStdString(name));
//         layout->addWidget(playerLabel);
//     }
//
//     layout->addStretch();
// }
//
// void WaitingRoomWindow::run_waiting_loop() {
//     while (!salir_de_sala) {
//         ServerMessage msg = server_handler.recv_response_from_server();
//
//         if (msg.type == ServerMessage::Type::Players) {
//             update_player_list(msg.players);
//         }
//         else if (msg.type == ServerMessage::Type::Start) {
//             QMessageBox::information(this, "Inicio", "El juego va a comenzar.");
//             break;
//         }
//         else if (msg.type == ServerMessage::Type::Closed) {
//             QMessageBox::warning(this, "Sala cerrada", "El host cerró la sala.");
//             break;
//         }
//     }
//
//     this->close();
// }
