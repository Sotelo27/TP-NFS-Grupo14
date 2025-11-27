#include "menu_screen.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

MenuScreen::MenuScreen(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(800, 600);
    setWindowTitle("Menú de Sala");

    QLabel* background = new QLabel(this);
    background->setPixmap(
        QPixmap("assets/images/nfs_most_wanted.png").scaled(
            size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation
        )
    );
    background->setGeometry(0, 0, width(), height());
    background->lower();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 40, 0);
    mainLayout->addStretch();

    jugarButton = new QPushButton("JUGAR", this);
    jugarButton->setFixedSize(260, 70);
    jugarButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 32px; font-weight: bold;"
        "  color: #F7B500;"
        "  background-color: transparent;"
        "  border: none;"
        "  text-shadow: 3px 3px #5A0080;"
        "  letter-spacing: 3px;"
        "}"
        "QPushButton:hover {"
        "  color: #FFD75F;"
        "  text-shadow: 4px 4px #7A00A8;"
        "}"
        "QPushButton:pressed {"
        "  color: #C89000;"
        "}"
    );

    seleccionarAutoButton = new QPushButton("SELECCIONAR AUTO", this);
    seleccionarAutoButton->setFixedSize(260, 55);
    seleccionarAutoButton->setStyleSheet(
        "QPushButton {"
        "  font-size: 22px; font-weight: bold;"
        "  color: #F7B500;"
        "  background-color: transparent;"
        "  border: none;"
        "  text-shadow: 3px 3px #5A0080;"
        "  letter-spacing: 2px;"
        "}"
        "QPushButton:hover {"
        "  color: #FFD75F;"
        "  text-shadow: 4px 4px #7A00A8;"
        "}"
    );

    mainLayout->addWidget(jugarButton, 0, Qt::AlignRight);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(seleccionarAutoButton, 0, Qt::AlignRight);
    mainLayout->addStretch();

    connect(jugarButton, &QPushButton::clicked, this, [this]() {
        emit go_to_lobby_screen();
    });
    connect(seleccionarAutoButton, &QPushButton::clicked, this, [this]() {
        emit go_to_selection_car_screen();
    });
}
