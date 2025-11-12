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

class LoginScreen : public QWidget {
    Q_OBJECT
public:
    explicit LoginScreen(ServerHandler& server_handler, size_t& my_id,
                        QWidget* parent);
    ~LoginScreen() = default;

private slots:
    void onLoginClicked();

signals:
    void go_to_car_selection_screen();

private:
    ServerHandler& server_handler;
    size_t& my_id;
    QLabel* title;
    QLabel* usernameLabel;
    QLineEdit* usernameInput;
    QPushButton* loginButton;

    void setupUi();
    void setupStyles();
    void setupConnections();

    void createWindowBackground();
    void createTitle();
    void createUsernameLabel();
    void createButtonLogIn();
    void createContainer();
};

#endif // LOGIN_WINDOW_H
