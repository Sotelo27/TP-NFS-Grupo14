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

    QLabel *title = new QLabel("Log In", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; font-weight: bold;");

    QLabel *usernameLabel = new QLabel("Username:", this);
    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Enter your username");
    loginButton = new QPushButton("Ingresar", this);

    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(usernameInput);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(title);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(userLayout);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(loginButton, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    setLayout(mainLayout);
    setWindowTitle("Login");
    resize(300, 200);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

void LoginWindow::onLoginClicked() {
    QString username = usernameInput->text().trimmed();

    if (username.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debes ingresar un nombre de usuario.");
        return;
    }

    QMessageBox::information(this, "Bienvenido", "Has ingresado como: " + username);
    client.start();
}
