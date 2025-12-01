#include "game_window.h"
#include <QVBoxLayout>
#include <QFile>
#include <QDebug>
#include <QScreen>
#include <QApplication>
#include <QLabel>
#include <QPixmap>
#include <QSound>

GameWindow::GameWindow(ServerHandler& server_handler, size_t& my_id, bool& map_selected, MapID& selected_map_game, bool login, QWidget *parent)
    : QDialog(parent), server_handler(server_handler), my_id(my_id), map_selected(map_selected), selected_map_game(selected_map_game), sound(nullptr)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen ? screen->availableGeometry().size() : QSize(800,600);
    setFixedSize(screenSize);

    stack = new QStackedWidget(this);
    stack->setGeometry(0, 0, screenSize.width(), screenSize.height());

    setupScreens();
    setupConnections();
    setupSound();

    if (login)
        stack->setCurrentWidget(start_screen);
    else
        stack->setCurrentWidget(result_finish_screen);

    final_results.clear();
}

void GameWindow::setupScreens() {
    start_screen = new StartScreen(this);
    login_screen = new LoginScreen(server_handler, my_id, this);
    lobby_screen = new LobbyScreen(server_handler, my_id, this);
    waiting_room_screen = new WaitingRoomScreen(server_handler, my_id, map_selected, selected_map_game, this);
    selection_car_screen = new SelectionCarScreen(server_handler, this);
    selection_map_screen = new SelectionMapScreen(this);
    result_finish_screen = new ResultFinishScreen(server_handler, my_id, this);
    menu_screen = new MenuScreen(server_handler, this);
    editor_map_screen = new EditorMapScreen(server_handler, this);

    stack->addWidget(start_screen);
    stack->addWidget(login_screen);
    stack->addWidget(lobby_screen);
    stack->addWidget(waiting_room_screen);
    stack->addWidget(selection_car_screen);
    stack->addWidget(selection_map_screen);
    stack->addWidget(result_finish_screen);
    stack->addWidget(menu_screen);
    stack->addWidget(editor_map_screen);
}

void GameWindow::setupConnections() {
    connect(start_screen, &StartScreen::goToLoginScreen, this, &GameWindow::goToLogin);
    connect(login_screen, &LoginScreen::go_to_car_selection_screen, this, &GameWindow::goToCarSelection);

    connect(selection_car_screen, &SelectionCarScreen::car_selected, this, [this](CarSpriteID car_id){
        int idx = selection_car_screen->findCarIndexById(car_id);
        menu_screen->setSelectedCarIndex(idx);
        stack->setCurrentWidget(menu_screen);
    });

    connect(selection_car_screen, &SelectionCarScreen::car_selected, this, [this](CarSpriteID car_id){
        server_handler.send_choose_car(static_cast<uint8_t>(car_id));
    });

    connect(selection_car_screen, &SelectionCarScreen::go_to_menu, this, &GameWindow::goToMenu);
    connect(menu_screen, &MenuScreen::go_to_lobby_screen, this, &GameWindow::goToLobby);
    connect(menu_screen, &MenuScreen::go_to_selection_car_screen, this, [this](){
        selection_car_screen->setSelectedCarIndex(menu_screen->getSelectedCarIndex());
        stack->setCurrentWidget(selection_car_screen);
    });

    connect(lobby_screen, &LobbyScreen::go_to_waiting_room_screen, this, &GameWindow::goToWaitingRoom);
    connect(lobby_screen, &LobbyScreen::go_to_selection_map_screen, this, &GameWindow::goToMapSelection);
    connect(lobby_screen, &LobbyScreen::go_to_editor_screen, this, [this]() {
        editor_map_screen->start_polling();
        stack->setCurrentWidget(editor_map_screen);
    });

    connect(waiting_room_screen, &WaitingRoomScreen::go_to_selection_map_screen, this, [this](){
        if (waiting_room_screen->isAdmin()) goToMapSelection();
    });

    connect(waiting_room_screen, &WaitingRoomScreen::go_back_to_lobby_screen, this, [this](){
        stack->setCurrentWidget(lobby_screen);
        lobby_screen->on_return_from_waiting_room();
    });

    connect(waiting_room_screen, &WaitingRoomScreen::go_to_game_start, this, &GameWindow::close);

    connect(selection_map_screen, &SelectionMapScreen::go_to_waiting_room_screen, this, [this](){
        QString selected_map = selection_map_screen->get_selected_map();
        waiting_room_screen->set_selected_map(selected_map);
        goToWaitingRoomFromMap();
    });

    connect(editor_map_screen, &EditorMapScreen::go_back_to_menu, this, [this]() {
        waiting_room_screen->startPolling();
        stack->setCurrentWidget(waiting_room_screen);
    });

    connect(editor_map_screen, &EditorMapScreen::go_to_waiting_room, this,
            [this]() {
        waiting_room_screen->set_selected_map(editor_map_screen->get_file_selected());
        waiting_room_screen->hideSelectMapButton();
        waiting_room_screen->startPolling();
        stack->setCurrentWidget(waiting_room_screen);
    });
}

void GameWindow::setupSound() {
    QString audioPath = "assets/audio/sound_intro_nfs.wav";
    if (QFile::exists(audioPath)) {
        sound = new QSound(audioPath, this);
        sound->setLoops(QSound::Infinite);
        sound->play();
    }
}

GameWindow::~GameWindow() {
    if (sound) {
        sound->stop();
        delete sound;
    }
}

void GameWindow::setFinalResults(const std::vector<PlayerResultTotal>& results) {
    final_results = results;
    if (result_finish_screen) {
        result_finish_screen->setFinalResults(final_results);
    }
}

void GameWindow::goToLobby() {
    waiting_room_screen->stopPolling();
    lobby_screen->startPolling();
    stack->setCurrentWidget(lobby_screen);
}

void GameWindow::goToWaitingRoom() {
    waiting_room_screen->startPolling();
    stack->setCurrentWidget(waiting_room_screen);
}

void GameWindow::goToCarSelection() {
    stack->setCurrentWidget(selection_car_screen);
}

void GameWindow::goToMapSelection() {
    stack->setCurrentWidget(selection_map_screen);
}

void GameWindow::goToResults() {
    if (result_finish_screen) {
        result_finish_screen->setFinalResults(final_results);
    }
    stack->setCurrentWidget(result_finish_screen);
}

void GameWindow::goToLogin() {
    stack->setCurrentWidget(login_screen);
}

void GameWindow::goToWaitingRoomFromMap() {
    waiting_room_screen->startPolling();
    stack->setCurrentWidget(waiting_room_screen);
}

void GameWindow::goToMenu() {
    stack->setCurrentWidget(menu_screen);
}
