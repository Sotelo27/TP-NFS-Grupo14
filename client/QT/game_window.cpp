#include "game_window.h"

#include <QFile>

GameWindow::GameWindow(ServerHandler& server_handler, size_t& my_id, bool login, QWidget *parent)
    : QDialog(parent), server_handler(server_handler), my_id(my_id) {
    resize(800, 600);
    stack = new QStackedWidget(this);

    // QString audioPath = "assets/audio/sound_intro_nfs.wav";
    //
    // sound = new QSound(audioPath, this);
    //
    // if (!QFile::exists(audioPath)) {
    //     std::cout << "El archivo no existe en la ruta: " << audioPath.toStdString() << std::endl;
    // } else {
    //     std::cout << "Archivo encontrado!" << std::endl;
    //     sound->setLoops(QSound::Infinite);
    //     sound->play();
    // }

    login_screen = new LoginScreen(server_handler, my_id, this);
    lobby_screen = new LobbyScreen(server_handler, my_id, this);
    waiting_room_screen = new WaitingRoomScreen(server_handler, my_id, this);
    selection_car_screen = new SelectionCarScreen(server_handler, this);
    selection_map_screen = new SelectionMapScreen(server_handler, this);
    result_finish_screen = new ResultFinishScreen(server_handler, my_id, this);

    stack->addWidget(login_screen);
    stack->addWidget(lobby_screen);
    stack->addWidget(waiting_room_screen);
    stack->addWidget(selection_car_screen);
    stack->addWidget(selection_map_screen);
    stack->addWidget(result_finish_screen);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stack);
    setLayout(layout);

    if (login) {
        stack->setCurrentWidget(login_screen);
        connect(login_screen, &LoginScreen::go_to_car_selection_screen, this, &GameWindow::go_to_car_selection);
    } else {
        stack->setCurrentWidget(result_finish_screen);
    }
    connect(selection_car_screen, &SelectionCarScreen::go_to_lobby, this, &GameWindow::go_to_lobby);
    connect(lobby_screen, &LobbyScreen::go_to_waiting_room_screen, this, &GameWindow::go_to_waiting_room);
    connect(waiting_room_screen, &WaitingRoomScreen::go_to_selection_map_screen, this, &GameWindow::go_to_map_selection);
    connect(waiting_room_screen, &WaitingRoomScreen::go_back_to_lobby_screen, this, &GameWindow::go_to_lobby);
    connect(waiting_room_screen, &WaitingRoomScreen::go_to_game_start, this, &GameWindow::close);

}

GameWindow::~GameWindow() {
    if (sound) {
        sound->stop();
        delete sound;
    }
}

void GameWindow::go_to_lobby() const {
    lobby_screen->startPolling();
    stack->setCurrentWidget(lobby_screen);
}

void GameWindow::go_to_waiting_room() const {
    stack->setCurrentWidget(waiting_room_screen);
}

void GameWindow::go_to_car_selection() const {
    stack->setCurrentWidget(selection_car_screen);
}

void GameWindow::go_to_map_selection() const {
    stack->setCurrentWidget(selection_map_screen);
}

void GameWindow::go_to_results() const {
    stack->setCurrentWidget(result_finish_screen);
}

void GameWindow::go_to_login() const {
    stack->setCurrentWidget(login_screen);
}