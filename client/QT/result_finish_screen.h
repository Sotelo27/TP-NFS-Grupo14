#ifndef RESULT_FINISH_WINDOW_H
#define RESULT_FINISH_WINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

#include "../connection/server_handler.h"

class ResultFinishScreen : public QWidget {
    Q_OBJECT

public:
    explicit ResultFinishScreen(ServerHandler& server_handler, size_t& my_id, QWidget *parent = nullptr);

    signals:
        void send_id_car();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QLabel* background;
    QLabel* title_label;
    QTableWidget* table;
    QWidget* container;

    QVBoxLayout* mainLayout;

    void setupUi();
    void setupStyles();
    void populateTable();

    void createBackground();
    void createContainer();
    void createTitle();
    void createTable();
};

#endif // RESULT_FINISH_WINDOW_H
