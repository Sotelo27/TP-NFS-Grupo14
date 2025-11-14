#include "result_finish_screen.h"
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>

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
    setFixedSize(1100, 750);

    // ---- Fondo ----
    QPalette pal;
    pal.setBrush(QPalette::Window, QPixmap("assets/images/fondo.png"));
    setAutoFillBackground(true);
    setPalette(pal);

    // ---- Layout principal ----
    auto* layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(0, 40, 0, 0);

    // ---- Caja contenedora ----
    QWidget* container = new QWidget(this);
    container->setFixedSize(850, 580);
    container->setStyleSheet(
        "background-color: rgba(10, 0, 30, 200);"
        "border: 5px solid #ff33cc;"
        "border-radius: 15px;"
    );

    // Sombra neón
    auto* glow = new QGraphicsDropShadowEffect(this);
    glow->setBlurRadius(60);
    glow->setColor(QColor(255, 0, 180));
    glow->setOffset(0, 0);
    container->setGraphicsEffect(glow);

    auto* container_layout = new QVBoxLayout(container);
    container_layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    container_layout->setContentsMargins(20, 20, 20, 20);

    // ---- TÍTULO ----
    title_label = new QLabel("RANKING", container);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setFixedHeight(110);
    title_label->setStyleSheet(
        "background-color: rgba(255, 0, 180, 0.20);"
        "border: 4px solid #ff33cc;"
        "font-size: 60px;"
        "font-weight: bold;"
        "color: #ff66ff;"
        "border-radius: 10px;"
        "padding: 10px;"
        "text-shadow: 0 0 25px #ff33cc;"
    );

    // ---- Tabla ----
    table = new QTableWidget(10, 3, container);
    table->setFixedHeight(430);
    table->setHorizontalHeaderLabels({"POSICIÓN", "NOMBRE", "TIEMPO"});
    table->verticalHeader()->setVisible(false);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    container_layout->addWidget(title_label);
    container_layout->addSpacing(15);
    container_layout->addWidget(table);

    layout->addWidget(container);
}

void ResultFinishScreen::setup_style() {

    table->setStyleSheet(
        "QTableWidget {"
        "   background-color: rgba(20, 0, 40, 180);"
        "   border: 3px solid #ff33cc;"
        "   gridline-color: #ff33cc;"
        "   font-size: 26px;"
        "   color: #39ff14;"
        "}"
        "QHeaderView::section {"
        "   background-color: rgba(255, 0, 150, 200);"
        "   color: white;"
        "   font-size: 24px;"
        "   border: 2px solid #ff33cc;"
        "   padding: 6px;"
        "}"
        "QTableWidget::item {"
        "   padding: 12px;"
        "}"
    );
}

void ResultFinishScreen::populate_table() {
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
