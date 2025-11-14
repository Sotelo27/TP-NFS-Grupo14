#include "result_finish_screen.h"
#include <QHeaderView>

ResultFinishScreen::ResultFinishScreen(ServerHandler& server_handler,
                                       size_t& my_id,
                                       QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      my_id(my_id)
{
    setup_ui();
    setup_style();
    populate_table();
}

void ResultFinishScreen::setup_ui() {
    setFixedSize(900, 700);

    // Fondo de pantalla
    QPalette pal;
    pal.setBrush(QPalette::Window, QPixmap("assets/images/fondo.png"));
    setAutoFillBackground(true);
    setPalette(pal);

    // Layout
    auto* layout = new QVBoxLayout(this);

    // Título estilo neón
    title_label = new QLabel("RANKING", this);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setFixedHeight(120);
    title_label->setStyleSheet(
        "font-size: 60px; font-weight: bold; "
        "color: #ff1ef7; text-shadow: 0 0 20px #ff1ef7;"
    );

    layout->addWidget(title_label);

    // Tabla
    table = new QTableWidget(10, 3, this);
    table->setFixedSize(700, 500);
    table->setHorizontalHeaderLabels({"POSICIÓN", "NOMBRE", "TIEMPO"});
    table->verticalHeader()->setVisible(false);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    layout->addWidget(table);
}


void ResultFinishScreen::setup_style() {
    // Estilos neon tipo arcade
    table->setStyleSheet(
        "QTableWidget {"
        "   background-color: rgba(20, 0, 40, 180);"
        "   border: 4px solid #ff1ef7;"
        "   gridline-color: #ff1ef7;"
        "   font-size: 26px;"
        "   color: #39ff14;"
        "}"
        "QHeaderView::section {"
        "   background-color: rgba(255, 0, 150, 180);"
        "   color: white;"
        "   font-size: 22px;"
        "   border: 2px solid #ff1ef7;"
        "}"
    );
}


void ResultFinishScreen::populate_table() {
    // Esto después lo llenás con los datos reales que te pase el server
    // Por ahora te dejo un mock para que veas el estilo:

    QString nombres[10] = { "ALEX", "JAMIE", "MARÍA", "PEDRO", "SARA",
                            "CARLA", "MIGUEL", "LAURA", "LUIS", "TOMÁS" };

    QString tiempos[10] = { "1:20", "1:26", "1:33", "1:40", "1:45",
                            "1:50", "2:00", "2:10", "2:22", "2:30" };

    for (int i = 0; i < 10; i++) {
        auto* pos = new QTableWidgetItem(QString::number(i + 1) + "º");
        auto* name = new QTableWidgetItem(nombres[i]);
        auto* time = new QTableWidgetItem(tiempos[i]);

        pos->setTextAlignment(Qt::AlignCenter);
        name->setTextAlignment(Qt::AlignCenter);
        time->setTextAlignment(Qt::AlignCenter);

        table->setItem(i, 0, pos);
        table->setItem(i, 1, name);
        table->setItem(i, 2, time);
    }
}