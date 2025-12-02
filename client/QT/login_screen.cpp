#include "login_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMessageBox>
#include <QDebug>

LoginScreen::LoginScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), my_id(my_id)
{
    setupUi();
    setupConnections();
}

void LoginScreen::setupUi() {
    createBackground();
    createTitle();
    createUsernameInput();
    createLoginButton();
    createLayout();
    setupStyles();
}

void LoginScreen::createBackground() {
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/nfs_most_wanted.png"));
    background->setScaledContents(true);
    background->lower();
}

void LoginScreen::createTitle() {
    title = new QLabel("", this); // solo para mantener espacio
    title->setAlignment(Qt::AlignCenter);
}

void LoginScreen::createUsernameInput() {
    usernameLabel = new QLabel("USUARIO:", this);
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Ingresá tu nombre");
    usernameInput->setFixedWidth(400);
}

void LoginScreen::createLoginButton() {
    loginButton = new QPushButton("ENTRAR", this);
}

void LoginScreen::createLayout() {
    QHBoxLayout* userLayout = new QHBoxLayout();
    userLayout->addStretch();
    userLayout->addWidget(usernameLabel);
    userLayout->addSpacing(10);
    userLayout->addWidget(usernameInput);
    userLayout->addStretch();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch(1);
    mainLayout->addWidget(title, 0, Qt::AlignCenter);
    mainLayout->addSpacing(25);
    mainLayout->addLayout(userLayout);
    mainLayout->addSpacing(18);
    mainLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    mainLayout->addStretch(1);

    setLayout(mainLayout);
}

void LoginScreen::setupStyles() {
    usernameLabel->setStyleSheet(
        "font-size: 18px; font-weight: 700; color: #39FF14; text-shadow: 0 0 6px #FF00C8;"
    );
    usernameInput->setStyleSheet(
        "QLineEdit {"
        "  font-size: 18px; font-weight: 600; color: #00FFE2;"
        "  padding: 10px 14px; background: rgba(15,0,35,0.55);"
        "  border: 2px solid #7D00FF; border-radius: 10px;"
        "}"
        "QLineEdit:focus { border-color:#FF00C8; box-shadow: 0 0 8px #FF00C8; }"
    );
    loginButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 20px; font-weight: 800; letter-spacing: 2px; color: #0afff7;"
        "  padding: 14px 30px;"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7300FF, stop:1 #FF00C8);"
        "  border: 3px solid rgba(255,255,255,0.35); border-radius: 14px;"
        "  box-shadow: 0 0 14px #FF00C8;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF00C8, stop:1 #00FFE2);"
        "  box-shadow: 0 0 20px #00FFE2;"
        "}"
        "QPushButton:pressed { background:#280040; }"
    );
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

    qDebug() << "[Client] Enviando nombre de usuario:" << username;
    server_handler.send_username(username.toStdString());

    emit go_to_car_selection_screen();
}

// Este resizeEvent asegura que el fondo siempre llene el widget
void LoginScreen::resizeEvent(QResizeEvent* event) {
    if (background) {
        background->setGeometry(0, 0, width(), height());
    }
    QWidget::resizeEvent(event);
}
