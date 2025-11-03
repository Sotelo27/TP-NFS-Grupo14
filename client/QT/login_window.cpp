#include "login_window.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

LoginWindow::LoginWindow(const char* host,
                        const char* service,
                        QWidget* parent)
    : QWidget(parent), client(host, service)
{
    setWindowTitle("Login");
    setFixedSize(640, 360);
    QLabel* background = new QLabel(this);
    background->setPixmap(
        QPixmap("assets/images/nfs_most_wanted.png").scaled(
            size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
        )
    );
    background->setGeometry(0, 0, width(), height());
    background->lower();

    QLabel *title = new QLabel("Need For Speed\nLog In", this);
    title->setAlignment(Qt::AlignLeft|Qt::AlignTop);
    title->setStyleSheet("font-size: 40px; font-weight: bold; color: red;");
    title->setMinimumHeight(50);

    QLabel *usernameLabel = new QLabel("Username:", this);
    usernameLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Enter your username");
    usernameInput->setStyleSheet("font-size: 16px; padding: 5px; color: white; background-color: #1F2C4D; border: 1px solid white;");
    usernameInput->setMinimumHeight(30);

    loginButton = new QPushButton("Ingresar", this);
    loginButton->setStyleSheet("font-size: 16px; font-weight: bold; color: green; background-color: #2E3A5D; padding: 5px;");

    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(usernameInput);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(title);
    mainLayout->addSpacing(100);
    mainLayout->addLayout(userLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    setLayout(mainLayout);

    title->raise();
    usernameLabel->raise();
    usernameInput->raise();
    loginButton->raise();

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked() {
    QString username = usernameInput->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre de usuario.");
        return;
    }

    // TODO (protocolo): Enviar nombre al servidor antes de mostrar el lobby.
    // Por ejemplo podemos hacer un wrapper en la capa Client/ServerHandler:
    //   client.send_username(username.toStdString());
    //
    // TODO (protocolo): Solicitar/esperar el listado de salas (ServerMessage::Type::Rooms).
    //   - Al recibir Rooms (vector<RoomInfo>), poblar una vista en Qt (lista/botones).
    //   - Al presionar "Crear sala": client.create_room();  (internamente: ClientProtocol::send_create_room())
    //   - Al presionar "Unirse a sala": client.join_room(roomId); (internamente: ClientProtocol::send_join_room(roomId))
    //   - El server volverá a enviar Rooms para refrescar cupos o un OK; actualizar UI en consecuencia.
    //

    this->close();
    client.start();  // No toco el flujo actual del sdl.
}
