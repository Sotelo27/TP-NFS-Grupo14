#include "result_finish_screen.h"
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QLabel>
#include <QTableWidgetItem>
#include <QString>
#include <algorithm>
#include <iomanip>
#include <sstream>

ResultFinishScreen::ResultFinishScreen(ServerHandler& server_handler,
                                       size_t& my_id,
                                       QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      my_id(my_id)
{
    setup_ui();
    setup_style();
    // populate_table(); // No llamar aquí, se llamará cuando se reciban los resultados reales
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

    auto* header = table->horizontalHeader();
    header->setVisible(true);
    header->setMinimumHeight(55);
    header->setFixedHeight(55);
    header->setDefaultAlignment(Qt::AlignCenter);

    header->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: rgba(255, 0, 150, 200);"
        "   color: white;"
        "   font-size: 26px;"
        "   border: 2px solid #ff33cc;"
        "   padding: 10px;"
        "}"
    );

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);

    // ---- Layout ----
    container_layout->addWidget(title_label);
    container_layout->addSpacing(25);
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
        "QTableWidget::item {"
        "   padding: 12px;"
        "}"
    );
}

// Nuevo: setear resultados y poblar tabla
void ResultFinishScreen::set_results(const std::vector<PlayerResultCurrent>& results) {
    player_results = results;
    populate_table();
}

// Formatea segundos a "M:SS" (ej: 83 -> "1:23")
static QString format_seconds(uint32_t seconds) {
    uint32_t min = seconds / 60;
    uint32_t sec = seconds % 60;
    return QString("%1:%2").arg(min).arg(sec, 2, 10, QChar('0'));
}

void ResultFinishScreen::populate_table() {
    table->clearContents();

    // Ordenar por posición (por si acaso)
    std::vector<PlayerResultCurrent> sorted = player_results;
    std::sort(sorted.begin(), sorted.end(), [](const PlayerResultCurrent& a, const PlayerResultCurrent& b) {
        return a.position < b.position;
    });

    int row = 0;
    for (const auto& player : sorted) {
        auto* pos  = new QTableWidgetItem(QString::number(player.position) + "º");
        auto* name = new QTableWidgetItem(QString::fromStdString(player.username));
        auto* time = new QTableWidgetItem(format_seconds(player.race_time_seconds));

        pos->setTextAlignment(Qt::AlignCenter);
        name->setTextAlignment(Qt::AlignCenter);
        time->setTextAlignment(Qt::AlignCenter);

        table->setItem(row, 0, pos);
        table->setItem(row, 1, name);
        table->setItem(row, 2, time);
        row++;
    }

    // Limpiar filas restantes si hay menos jugadores que filas
    for (; row < table->rowCount(); ++row) {
        table->setItem(row, 0, new QTableWidgetItem(""));
        table->setItem(row, 1, new QTableWidgetItem(""));
        table->setItem(row, 2, new QTableWidgetItem(""));
    }
}
