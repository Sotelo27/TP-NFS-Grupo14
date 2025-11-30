#ifndef LOGIN_SCREEN_H
#define LOGIN_SCREEN_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include "../connection/server_handler.h"

class LoginScreen : public QWidget {
    Q_OBJECT
public:
    explicit LoginScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent = nullptr);
    ~LoginScreen() = default;

    signals:
        void go_to_car_selection_screen();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onLoginClicked();

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QLabel* background = nullptr;
    QLabel* title = nullptr;
    QLabel* usernameLabel = nullptr;
    QLineEdit* usernameInput = nullptr;
    QPushButton* loginButton = nullptr;

    void setupUi();
    void setupStyles();
    void setupConnections();

    void createBackground();
    void createTitle();
    void createUsernameInput();
    void createLoginButton();
    void createLayout();
};

#endif // LOGIN_SCREEN_H
