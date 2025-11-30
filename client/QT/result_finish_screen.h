#ifndef RESULT_FINISH_WINDOW_H
#define RESULT_FINISH_WINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <vector>
#include <string>
#include "../../common/dto/results_info.h"
#include "../connection/server_handler.h"

class ResultFinishScreen : public QWidget {
    Q_OBJECT

public:
    ResultFinishScreen(ServerHandler& server_handler, size_t& my_id, QWidget *parent = nullptr);

    // Nuevo: setear resultados de jugadores
    void set_results(const std::vector<PlayerResultCurrent>& results);

signals:
    void send_id_car();

private:
    ServerHandler& server_handler;
    size_t& my_id;

    QLabel* title_label;
    QTableWidget* table;

    std::vector<PlayerResultCurrent> player_results;

    void setup_ui();
    void setup_style();
    void populate_table();
};

#endif // RESULT_FINISH_WINDOW_H