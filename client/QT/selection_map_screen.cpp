#include "selection_map_screen.h"

SelectionMapScreen::SelectionMapScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler) {}