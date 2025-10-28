#ifndef QT_H
#define QT_H

#include <QWidget>
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>

#include "client/client.h"

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    explicit LoginWindow(const char* host,
                        const char* service,
                        QWidget* parent = nullptr);
    // int run();
    ~LoginWindow() = default;

private slots:
    void onLoginClicked();

private:
    QLineEdit *usernameInput;
    QPushButton *loginButton;
    Client client;
 };


#endif //QT_H