#include "login_screen.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPixmap>
#include "lobby_screen.h"

#define WIDTH_SIZE_WINDOW 800
#define HEIGHT_SIZE_WINDOW 600

LoginScreen::LoginScreen(ServerHandler& server_handler, size_t& my_id,
                         QWidget* parent)
    : QWidget(parent), server_handler(server_handler), my_id(my_id)
{
    setupUi();
    setupConnections();
}

void LoginScreen::createTitle() {
    // Título vacío (elimino asi se ve el arte del fondo no mas)
    title = new QLabel("", this);
    title->setFixedHeight(30);
    title->setAlignment(Qt::AlignCenter);
}

void LoginScreen::createWindowBackground() {
    QLabel* background = new QLabel(this);
    background->setPixmap(
        QPixmap("assets/images/nfs_most_wanted.png").scaled( // asegurar misma imagen que StartScreen
            size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
        )
    );
    background->setGeometry(0, 0, width(), height());
    background->lower();
}

void LoginScreen::createUsernameLabel() {
    usernameLabel = new QLabel("USUARIO:", this);
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Ingresá tu nombre");
    usernameInput->setMinimumHeight(42);
}

void LoginScreen::createButtonLogIn() {
    loginButton = new QPushButton("ENTRAR", this);
}

void LoginScreen::createContainer() {
    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(usernameInput);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    // ya no se agrega texto del título, sólo un pequeño espacio
    mainLayout->addWidget(title, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25);
    mainLayout->addLayout(userLayout);
    mainLayout->addSpacing(18);
    mainLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

void LoginScreen::setupStyles() {
    // Vaporwave palette
    usernameLabel->setStyleSheet(
        "font-size: 18px; font-weight: 700; letter-spacing: 1px;"
        "color: #FF00C8; text-shadow: 0 0 6px #FF00C8;"
    );
    usernameInput->setStyleSheet(
        "QLineEdit {"
        "  font-size: 18px; font-weight: 600; color: #00FFE2;"
        "  padding: 10px 14px;"
        "  background: rgba(15,0,35,0.55);"
        "  border: 2px solid #7D00FF;"
        "  border-radius: 10px;"
        "}"
        "QLineEdit:focus {"
        "  border-color:#FF00C8;"
        "  box-shadow: 0 0 8px #FF00C8;"
        "}"
    );
    loginButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 20px; font-weight: 800; letter-spacing: 2px;"
        "  color: #0afff7;"
        "  padding: 14px 30px;"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "       stop:0 #7300FF, stop:1 #FF00C8);"
        "  border: 3px solid rgba(255,255,255,0.35);"
        "  border-radius: 14px;"
        "  box-shadow: 0 0 14px #FF00C8;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
        "       stop:0 #FF00C8, stop:1 #00FFE2);"
        "  box-shadow: 0 0 20px #00FFE2;"
        "}"
        "QPushButton:pressed {"
        "  background:#280040;"
        "}"
    );
}

void LoginScreen::setupUi() {
    setWindowTitle("Login");
    setFixedSize(WIDTH_SIZE_WINDOW, HEIGHT_SIZE_WINDOW);

    createWindowBackground();
    createTitle();
    createUsernameLabel();
    createButtonLogIn();
    createContainer();

    title->raise();
    usernameLabel->raise();
    usernameInput->raise();
    loginButton->raise();

    setupStyles();
}

void LoginScreen::setupConnections() {
    connect(loginButton, &QPushButton::clicked, this, &LoginScreen::onLoginClicked);
}

void LoginScreen::onLoginClicked() {
    QString username = usernameInput->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre de usuario.");
        return;
    }

    std::cout << "[Client] Enviando nombre de usuario: " << username.toStdString() << std::endl;
    server_handler.send_username(username.toStdString());

    emit go_to_car_selection_screen();

}


