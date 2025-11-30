#include "start_screen.h"
#include <QVBoxLayout>
#include <QApplication>

StartScreen::StartScreen(QWidget* parent)
    : QWidget(parent)
{
    // Fondo
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/nfs_most_wanted.png"));
    background->setScaledContents(true);
    background->lower(); // siempre detrás

    setupUI();
    setupConnections();
}

void StartScreen::setupUI() {
    // Layout principal para los botones
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();

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
    mainLayout->addWidget(playBtn, 0, Qt::AlignCenter);
    mainLayout->addSpacing(10);

    // Botón Salir
    quitBtn = new QPushButton("SALIR", this);
    quitBtn->setCursor(Qt::PointingHandCursor);
    quitBtn->setStyleSheet(
        "QPushButton { font-size: 14px; font-weight: 600; color: #FFFFFF;"
        "background-color: rgba(20,0,45,0.55); padding: 8px 22px;"
        "border: 2px solid rgba(255,0,200,0.4); border-radius: 10px; }"
        "QPushButton:hover { color:#00FFE2; border-color:#00FFE2; }"
    );
    mainLayout->addWidget(quitBtn, 0, Qt::AlignCenter);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

void StartScreen::setupConnections() {
    connect(playBtn, &QPushButton::clicked, this, &StartScreen::goToLoginScreen);
    connect(quitBtn, &QPushButton::clicked, []() { qApp->quit(); });
}

// Sobreescribimos resizeEvent para que el fondo siempre llene toda la ventana
void StartScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background) {
        background->setGeometry(0, 0, width(), height());
    }
}
