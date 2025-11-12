#include "result_finish_screen.h"

ResultFinishScreen::ResultFinishScreen(ServerHandler& server_handler, size_t& my_id, QWidget* parent)
    : QWidget(parent), server_handler(server_handler), my_id(my_id) {

}