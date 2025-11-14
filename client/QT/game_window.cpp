#include "game_window.h"

#include <QFile>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

// Pantalla inicial antes del Login: usa el mismo fondo 
class GameWindow::StartScreen : public QWidget {
    Q_OBJECT
public:
    explicit StartScreen(QWidget* parent = nullptr) : QWidget(parent) {
        setFixedSize(800, 600);

        // Fondo 
        QLabel* background = new QLabel(this);
        background->setPixmap(
            QPixmap("assets/images/nfs_most_wanted.png").scaled( 
                size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
            )
        );
        background->setGeometry(0, 0, width(), height());
        background->lower();

        // Spacer para mantener balance vertical sin título
        QWidget* spacer = new QWidget(this);
        spacer->setFixedHeight(40);

        // Botón Jugar con nuevo estilo
        QPushButton* playBtn = new QPushButton("INICIAR", this);
        playBtn->setCursor(Qt::PointingHandCursor);
        playBtn->setStyleSheet(
            "QPushButton {"
            "  font-size: 26px; font-weight: 800;"
            "  color: #0afff7;"
            "  padding: 18px 34px;"
            "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "    stop:0 #7300FF, stop:1 #FF00C8);"
            "  border: 3px solid rgba(255,255,255,0.3);"
            "  border-radius: 14px;"
            "  letter-spacing: 2px;"
            "  box-shadow: 0 0 12px #FF00C8;"
            "}"
            "QPushButton:hover {"
            "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "    stop:0 #FF00C8, stop:1 #00FFE2);"
            "  box-shadow: 0 0 18px #00FFE2;"
            "}"
            "QPushButton:pressed {"
            "  background-color: #280040;"
            "}"
        );

        // Botón Salir secundario
        QPushButton* quitBtn = new QPushButton("SALIR", this);
        quitBtn->setCursor(Qt::PointingHandCursor);
        quitBtn->setStyleSheet(
            "QPushButton {"
            "  font-size: 14px; font-weight: 600;"
            "  color: #FFFFFF;"
            "  background-color: rgba(20,0,45,0.55);"
            "  padding: 8px 22px;"
            "  border: 2px solid rgba(255,0,200,0.4);"
            "  border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "  color:#00FFE2;"
            "  border-color:#00FFE2;"
            "}"
        );

        // Layout
        QVBoxLayout* mainLayout = new QVBoxLayout();
        mainLayout->addStretch();
        mainLayout->addWidget(spacer);
        mainLayout->addSpacing(15);
        mainLayout->addWidget(playBtn, 0, Qt::AlignCenter);
        mainLayout->addSpacing(10);
        mainLayout->addWidget(quitBtn, 0, Qt::AlignCenter);
        mainLayout->addStretch();
        setLayout(mainLayout);

        // Conexiones
        connect(playBtn, &QPushButton::clicked, this, [this]() { emit go_to_login_screen(); });
        connect(quitBtn, &QPushButton::clicked, this, []() { qApp->quit(); });
    }

signals:
    void go_to_login_screen();
};

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

    // NUEVO: pantalla inicial
    start_screen = new StartScreen(this);

    login_screen = new LoginScreen(server_handler, my_id, this);
    lobby_screen = new LobbyScreen(server_handler, my_id, this);
    waiting_room_screen = new WaitingRoomScreen(server_handler, my_id, this);
    selection_car_screen = new SelectionCarScreen(server_handler, this);
    selection_map_screen = new SelectionMapScreen(server_handler, this);
    result_finish_screen = new ResultFinishScreen(server_handler, my_id, this);

    stack->addWidget(start_screen);
    stack->addWidget(login_screen);
    stack->addWidget(lobby_screen);
    stack->addWidget(waiting_room_screen);
    stack->addWidget(selection_car_screen);
    stack->addWidget(selection_map_screen);
    stack->addWidget(result_finish_screen);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stack);
    setLayout(layout);

    // Iniciar en la StartScreen 
    if (login) {
        stack->setCurrentWidget(start_screen);
        connect(start_screen, &StartScreen::go_to_login_screen, this, &GameWindow::go_to_login);
        connect(login_screen, &LoginScreen::go_to_car_selection_screen, this, &GameWindow::go_to_car_selection);
    } else {
        stack->setCurrentWidget(result_finish_screen);
    }

    connect(selection_car_screen, &SelectionCarScreen::car_selected, this, [this, &server_handler](CarSpriteID car_id) {
        std::cout << "[GameWindow] Enviando selección de auto al servidor: " << static_cast<int>(car_id) << std::endl;
        server_handler.send_choose_car(static_cast<uint8_t>(car_id));
    });
    
    connect(selection_car_screen, &SelectionCarScreen::go_to_lobby, this, &GameWindow::go_to_lobby);
    connect(lobby_screen, &LobbyScreen::go_to_waiting_room_screen, this, &GameWindow::go_to_waiting_room);
    connect(lobby_screen, &LobbyScreen::go_to_selection_map_screen, this, &GameWindow::go_to_map_selection); // NUEVO
    connect(waiting_room_screen, &WaitingRoomScreen::go_to_selection_map_screen, this, [this]() {
        // Solo el admin debe navegar a la selección de mapa
        if (waiting_room_screen->isAdmin()) {
            go_to_map_selection();
        }
        // Los demás no hacen nada, esperan el RaceStart
    });
    connect(waiting_room_screen, &WaitingRoomScreen::go_back_to_lobby_screen, this, [this]() {
        std::cout << "[GameWindow] Volviendo al lobby desde WaitingRoom\n";
        stack->setCurrentWidget(lobby_screen);
        lobby_screen->on_return_from_waiting_room(); 
    });
    connect(waiting_room_screen, &WaitingRoomScreen::go_to_game_start, this, &GameWindow::close);

    // NUEVO: conectar selección de mapa a sala de espera
    connect(selection_map_screen, &SelectionMapScreen::go_to_waiting_room_screen, this, [this]() {
        // Guardar el mapa seleccionado para usarlo luego en la sala de espera
        QString selected_map = selection_map_screen->get_selected_map();
        waiting_room_screen->set_selected_map(selected_map); // (opcional, para mostrar el nombre)
        go_to_waiting_room_from_map();
    });
}

GameWindow::~GameWindow() {
    if (sound) {
        sound->stop();
        delete sound;
    }
}

void GameWindow::go_to_lobby() const {
    // Detener polling de la waiting room al volver
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
    stack->setCurrentWidget(waiting_room_screen);
}

#include "game_window.moc"
