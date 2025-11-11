#ifndef RESULT_FINISH_WINDOW_H
#define RESULT_FINISH_WINDOW_H
#include <QWidget>

#include "../connection/server_handler.h"


class ResultFinishScreen : public QWidget {
    Q_OBJECT
    public:
    ResultFinishScreen(ServerHandler& server_handler, size_t& my_id, QWidget *parent);

    private:
    ServerHandler& server_handler;
    size_t& my_id;

    signals:
    void send_id_car();
};


#endif //RESULT_FINISH_WINDOW_H