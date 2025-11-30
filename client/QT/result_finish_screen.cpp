#include "result_finish_screen.h"
#include <QHeaderView>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>
#include <QPalette>
#include <QString>
#include <iomanip>
#include <sstream>

ResultFinishScreen::ResultFinishScreen(ServerHandler& server_handler,
                                       size_t& my_id,
                                       QWidget* parent)
    : QWidget(parent),
      server_handler(server_handler),
      my_id(my_id)
{
    setupUi();
    setupStyles();
    // No llamar a populateTable aquí, se llamará cuando se tengan los resultados reales
}

void ResultFinishScreen::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    createBackground();
    createContainer();
    createTitle();
    createTable();

    mainLayout->addWidget(container);
}

void ResultFinishScreen::createBackground() {
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->lower(); // siempre detrás
}

void ResultFinishScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background)
        background->setGeometry(0, 0, width(), height());
}

void ResultFinishScreen::createContainer() {
    container = new QWidget(this);
    container->setFixedSize(850, 580);
    container->setStyleSheet(
        "background-color: rgba(10, 0, 30, 200);"
        "border: 5px solid #ff33cc;"
        "border-radius: 15px;"
    );

    auto* glow = new QGraphicsDropShadowEffect(this);
    glow->setBlurRadius(60);
    glow->setColor(QColor(255, 0, 180));
    glow->setOffset(0, 0);
    container->setGraphicsEffect(glow);

    auto* containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(25);
    containerLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

void ResultFinishScreen::createTitle() {
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

    container->layout()->addWidget(title_label);
}

void ResultFinishScreen::createTable() {
    table = new QTableWidget(0, 3, container); // filas dinámicas
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

    container->layout()->addWidget(table);
}

void ResultFinishScreen::setupStyles() {
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

// NUEVO: Recibe y muestra los resultados finales
void ResultFinishScreen::setFinalResults(const std::vector<PlayerResultTotal>& results) {
    final_results = results;
    populateTable();
}

void ResultFinishScreen::populateTable() {
    table->setRowCount(static_cast<int>(final_results.size()));

    for (int i = 0; i < static_cast<int>(final_results.size()); ++i) {
        const auto& entry = final_results[i];

        // POSICIÓN
        auto* pos  = new QTableWidgetItem(QString::number(entry.position) + "º");
        pos->setTextAlignment(Qt::AlignCenter);

        // NOMBRE
        auto* name = new QTableWidgetItem(QString::fromStdString(entry.username));
        name->setTextAlignment(Qt::AlignCenter);

        // TIEMPO (formato mm:ss.zzz)
        int total_ms = static_cast<int>(entry.total_time_seconds * 1000);
        int minutes = total_ms / 60000;
        int seconds = (total_ms / 1000) % 60;
        int millis  = total_ms % 1000;
        QString time_str = QString("%1:%2.%3")
            .arg(minutes)
            .arg(seconds, 2, 10, QChar('0'))
            .arg(millis, 3, 10, QChar('0'));

        auto* time = new QTableWidgetItem(time_str);
        time->setTextAlignment(Qt::AlignCenter);

        table->setItem(i, 0, pos);
        table->setItem(i, 1, name);
        table->setItem(i, 2, time);
    }
}
