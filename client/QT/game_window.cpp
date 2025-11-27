#include "game_window.h"
#include "menu_screen.h"

#include <QFile>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScreen>

// Pantalla inicial antes del Login
class GameWindow::StartScreen : public QWidget {
    Q_OBJECT
public:
    explicit StartScreen(QWidget* parent = nullptr) : QWidget(parent) {

        // Fondo
        background = new QLabel(this);
        background->setScaledContents(true); // escalable automáticamente
        background->lower();

        // Spacer para balance vertical
        QWidget* spacer = new QWidget(this);
        spacer->setFixedHeight(40);

        // Botón Jugar
        playBtn = new QPushButton("INICIAR", this);
        playBtn->setCursor(Qt::PointingHandCursor);
        playBtn->setStyleSheet(
            "QPushButton { font-size: 26px; font-weight: 800; color: #0afff7;"
            "padding: 18px 34px; background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "stop:0 #7300FF, stop:1 #FF00C8); border: 3px solid rgba(255,255,255,0.3);"
            "border-radius: 14px; letter-spacing: 2px; box-shadow: 0 0 12px #FF00C8; }"
            "QPushButton:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "stop:0 #FF00C8, stop:1 #00FFE2); box-shadow: 0 0 18px #00FFE2; }"
            "QPushButton:pressed { background-color: #280040; }"
        );

        // Botón Salir
        quitBtn = new QPushButton("SALIR", this);
        quitBtn->setCursor(Qt::PointingHandCursor);
        quitBtn->setStyleSheet(
            "QPushButton { font-size: 14px; font-weight: 600; color: #FFFFFF;"
            "background-color: rgba(20,0,45,0.55); padding: 8px 22px;"
            "border: 2px solid rgba(255,0,200,0.4); border-radius: 10px; }"
            "QPushButton:hover { color:#00FFE2; border-color:#00FFE2; }"
        );

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addStretch();
        mainLayout->addWidget(spacer);
        mainLayout->addSpacing(15);
        mainLayout->addWidget(playBtn, 0, Qt::AlignCenter);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(quitBtn, 0, Qt::AlignCenter);
        mainLayout->addStretch();
        setLayout(mainLayout);

        connect(playBtn, &QPushButton::clicked, this, [this]() { emit go_to_login_screen(); });
        connect(quitBtn, &QPushButton::clicked, this, []() { qApp->quit(); });
    }

    void resizeEvent(QResizeEvent* event) override {
        QWidget::resizeEvent(event);
        if (background) {
            background->setPixmap(QPixmap("assets/images/nfs_most_wanted.png").scaled(
                size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
            ));
            background->setGeometry(0, 0, width(), height());
        }
    }

signals:
    void go_to_login_screen();

private:
    QLabel* background;
    QPushButton* playBtn;
    QPushButton* quitBtn;
};

// -----------------------------------------------------------
//  GameWindow
// -----------------------------------------------------------
GameWindow::GameWindow(ServerHandler& server_handler, size_t& my_id, bool login, QWidget *parent)
    : QDialog(parent), server_handler(server_handler), my_id(my_id) {

    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setMinimumSize(800, 600);
    resize(800, 600);

    stack = new QStackedWidget(this);

    // QString audioPath = "assets/audio/sound_intro_nfs.wav";
    // sound = new QSound(audioPath, this);
    //
    // if (QFile::exists(audioPath)) {
    //     sound->setLoops(QSound::Infinite);
    //     sound->play();
    // }

    start_screen = new StartScreen(this);

    login_screen = new LoginScreen(server_handler, my_id, this);
    lobby_screen = new LobbyScreen(server_handler, my_id, this);
    waiting_room_screen = new WaitingRoomScreen(server_handler, my_id, this);
    selection_car_screen = new SelectionCarScreen(this);
    selection_map_screen = new SelectionMapScreen(server_handler, this);
    result_finish_screen = new ResultFinishScreen(server_handler, my_id, this);
    menu_screen = new MenuScreen(this);
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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stack);
    setLayout(layout);

    if (login) {
        stack->setCurrentWidget(start_screen);
        connect(start_screen, &StartScreen::go_to_login_screen, this, &GameWindow::go_to_login);
        connect(login_screen, &LoginScreen::go_to_car_selection_screen, this, &GameWindow::go_to_car_selection);
    } else {
        stack->setCurrentWidget(result_finish_screen);
    }

    connect(selection_car_screen, &SelectionCarScreen::car_selected, this, [this](CarSpriteID car_id) {
        int idx = selection_car_screen->findCarIndexById(car_id);
        menu_screen->setSelectedCarIndex(idx);
        stack->setCurrentWidget(menu_screen);
    });
    connect(selection_car_screen, &SelectionCarScreen::car_selected, this, [this, &server_handler](CarSpriteID car_id) {
        server_handler.send_choose_car(static_cast<uint8_t>(car_id));
    });
    connect(selection_car_screen, &SelectionCarScreen::go_to_menu, this, &GameWindow::go_to_menu);

    connect(menu_screen, &MenuScreen::go_to_lobby_screen, this, &GameWindow::go_to_lobby);
    connect(menu_screen, &MenuScreen::go_to_selection_car_screen, this, [this]() {
        selection_car_screen->setSelectedCarIndex(menu_screen->getSelectedCarIndex());
        stack->setCurrentWidget(selection_car_screen);
    });

    connect(lobby_screen, &LobbyScreen::go_to_waiting_room_screen, this, &GameWindow::go_to_waiting_room);
    connect(lobby_screen, &LobbyScreen::go_to_selection_map_screen, this, &GameWindow::go_to_map_selection);
    connect(lobby_screen, &LobbyScreen::go_to_maps_created_from_editor_screen, this, &GameWindow::go_to_maps_edited_screen);

    connect(waiting_room_screen, &WaitingRoomScreen::go_to_selection_map_screen, this, [this]() {
        if (waiting_room_screen->isAdmin()) go_to_map_selection();
    });
    connect(waiting_room_screen, &WaitingRoomScreen::go_back_to_lobby_screen, this, [this]() {
        stack->setCurrentWidget(lobby_screen);
        lobby_screen->on_return_from_waiting_room();
    });
    connect(waiting_room_screen, &WaitingRoomScreen::go_to_game_start, this, &GameWindow::close);

    connect(selection_map_screen, &SelectionMapScreen::go_to_waiting_room_screen, this, [this]() {
        QString selected_map = selection_map_screen->get_selected_map();
        waiting_room_screen->set_selected_map(selected_map);
        go_to_waiting_room_from_map();
    });

    connect(editor_map_screen, &EditorMapScreen::go_to_waiting_room_screen, this, [this]() {
        waiting_room_screen->fromEditorScreen(true);
        waiting_room_screen->startPolling();
        stack->setCurrentWidget(waiting_room_screen);
    });

    connect(editor_map_screen, &EditorMapScreen::go_back_to_lobby, this, [this]() {
        stack->setCurrentWidget(lobby_screen);
    });

}

// -----------------------------------------------------------
void GameWindow::resizeEvent(QResizeEvent* event) {
    QDialog::resizeEvent(event);
    // El stack se ajusta automáticamente
}

// -----------------------------------------------------------
GameWindow::~GameWindow() {
    if (sound) {
        sound->stop();
        delete sound;
    }
}

// Slots
void GameWindow::go_to_lobby() const {
    waiting_room_screen->stopPolling();
    lobby_screen->startPolling();
    stack->setCurrentWidget(lobby_screen);
}

void GameWindow::go_to_waiting_room() const {
    waiting_room_screen->startPolling();
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

void GameWindow::go_to_waiting_room_from_map() const {
    waiting_room_screen->startPolling();
    waiting_room_screen->set_selected_map(selection_map_screen->get_selected_map());
    stack->setCurrentWidget(waiting_room_screen);
}

void GameWindow::go_to_menu() const {
    stack->setCurrentWidget(menu_screen);
}

void GameWindow::go_to_maps_edited_screen() const {
    editor_map_screen->load_maps_from_directory("editor/MapsEdited/");
    stack->setCurrentWidget(editor_map_screen);
}

#include "game_window.moc"
