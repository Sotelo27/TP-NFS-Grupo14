#ifndef RESULT_FINISH_WINDOW_H
#define RESULT_FINISH_WINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

#include "../connection/server_handler.h"

class ResultFinishScreen : public QWidget {
    Q_OBJECT

public:
    ResultFinishScreen(ServerHandler& server_handler, size_t& my_id, QWidget *parent = nullptr);
    void setResults(const std::vector<PlayerResultTotal>& results);

signals:
    void send_id_car();

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QLabel* title_label;
    QTableWidget* table;

    void setup_ui();
    void setup_style();
    void populate_table();
    void populate_table(const std::vector<PlayerResultTotal>& results);
};

#endif // RESULT_FINISH_WINDOW_H