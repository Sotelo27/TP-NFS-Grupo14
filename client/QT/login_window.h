#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QWidget>
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../../common/queue.h"
#include "../client_protocol.h"
#include "../connection/server_handler.h"

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(ServerHandler& server_handler,
                        QWidget* parent = nullptr);
    ~LoginWindow() = default;

private slots:
    void onLoginClicked();

private:
    ServerHandler& server_handler;
    QLabel* title;
    QLabel* usernameLabel;
    QLineEdit* usernameInput;
    QPushButton* loginButton;

    void setupUi();
    void setupStyles();
    void setupConnections();

    void createWindowBackground();
    void createTittle();
    void createUsernameLabel();
    void createButtonLogIn();
    void createContainer();
};

#endif // LOGIN_WINDOW_H
