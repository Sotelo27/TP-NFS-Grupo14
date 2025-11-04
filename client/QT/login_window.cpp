#include "login_window.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPixmap>
#include "lobby_window.h"

#define WIDTH_SIZE_WINDOW 800
#define HEIGHT_SIZE_WINDOW 560

LoginWindow::LoginWindow(ServerHandler& server_handler,
                         QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    setupUi();
    setupConnections();
}

void LoginWindow::createTittle() {
    title = new QLabel("Need For Speed\nLog In", this);
    title->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    title->setMinimumHeight(50);
}

void LoginWindow::createWindowBackground() {
    QLabel* background = new QLabel(this);
    background->setPixmap(
        QPixmap("assets/images/nfs_most_wanted.png").scaled(
            size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
        )
    );
    background->setGeometry(0, 0, width(), height());
    background->lower();
}

void LoginWindow::createUsernameLabel() {
    usernameLabel = new QLabel("Username:", this);
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Enter your username");
    usernameInput->setMinimumHeight(30);
}

void LoginWindow::createButtonLogIn() {
    loginButton = new QPushButton("Ingresar", this);
}

void LoginWindow::createContainer() {
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
}

void LoginWindow::setupStyles() {
    title->setStyleSheet("font-size: 40px; font-weight: bold; color: red;");
    usernameLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    usernameInput->setStyleSheet(
        "font-size: 16px; padding: 5px; color: white;"
        "background-color: #1F2C4D; border: 1px solid white;"
    );
    loginButton->setStyleSheet(
        "font-size: 16px; font-weight: bold; color: green;"
        "background-color: #2E3A5D; padding: 5px;"
    );
}

void LoginWindow::setupUi() {
    setWindowTitle("Login");
    setFixedSize(WIDTH_SIZE_WINDOW, HEIGHT_SIZE_WINDOW);

    createWindowBackground();
    createTittle();
    createUsernameLabel();
    createButtonLogIn();
    createContainer();

    title->raise();
    usernameLabel->raise();
    usernameInput->raise();
    loginButton->raise();

    setupStyles();
}

void LoginWindow::setupConnections() {
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked() {
    QString username = usernameInput->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre de usuario.");
        return;
    }

    std::cout << "[Client] Enviando nombre de usuario: " << username.toStdString() << std::endl;
    server_handler.send_username(username.toStdString());

    // Pequeña pausa para dar tiempo al servidor a procesar el nombre
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    LobbyWindow* lobby = new LobbyWindow(server_handler);
    lobby->show();
    
    this->close();
    
    // Ejecutar el loop del lobby para procesar mensajes
    std::cout << "[Client] Iniciando loop del lobby..." << std::endl;
    lobby->run_lobby_loop();
}
