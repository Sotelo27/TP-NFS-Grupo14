#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>

class StartScreen : public QWidget {
    Q_OBJECT
public:
    explicit StartScreen(QWidget* parent = nullptr);

    signals:
        void goToLoginScreen();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUI();
    void setupConnections();

    QLabel* background = nullptr;
    QPushButton* playBtn = nullptr;
    QPushButton* quitBtn = nullptr;
};

#endif // START_SCREEN_H
