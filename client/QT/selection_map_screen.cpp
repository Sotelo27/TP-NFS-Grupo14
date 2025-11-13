#include "selection_map_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QSpacerItem>
#include <QSizePolicy>

SelectionMapScreen::SelectionMapScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    setMinimumSize(800, 600);
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Seleccionar Mapa", this);
    title->setStyleSheet(
        "font-size: 24px; font-weight: 700;"
        "color: #B27CE8; padding: 8px;"
        "background-color: rgba(255,255,255,0.35);"
        "border: 1px solid rgba(178,124,232,0.4); border-radius: 10px;"
    );
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    mainLayout->addSpacing(30);

    // Layout horizontal para los mapas
    QHBoxLayout* mapsLayout = new QHBoxLayout();

    // --- Mapa: Liberty City ---
    QVBoxLayout* libertyLayout = new QVBoxLayout();
    QLabel* imgLiberty = new QLabel(this);
    imgLiberty->setPixmap(QPixmap("assets/cities/Liberty City.png").scaled(180, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imgLiberty->setAlignment(Qt::AlignCenter);
    QLabel* lblLiberty = new QLabel("Liberty City", this);
    lblLiberty->setAlignment(Qt::AlignCenter);
    QPushButton* btnLiberty = new QPushButton("Elegir", this);
    connect(btnLiberty, &QPushButton::clicked, this, [this]() { on_map_selected("LibertyCity"); });
    libertyLayout->addWidget(imgLiberty);
    libertyLayout->addWidget(lblLiberty);
    libertyLayout->addWidget(btnLiberty);

    // --- Mapa: San Andreas ---
    QVBoxLayout* sanLayout = new QVBoxLayout();
    QLabel* imgSan = new QLabel(this);
    imgSan->setPixmap(QPixmap("assets/cities/San Andreas.png").scaled(180, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imgSan->setAlignment(Qt::AlignCenter);
    QLabel* lblSan = new QLabel("San Andreas", this);
    lblSan->setAlignment(Qt::AlignCenter);
    QPushButton* btnSan = new QPushButton("Elegir", this);
    connect(btnSan, &QPushButton::clicked, this, [this]() { on_map_selected("SanAndreas"); });
    sanLayout->addWidget(imgSan);
    sanLayout->addWidget(lblSan);
    sanLayout->addWidget(btnSan);

    // --- Mapa: Vice City ---
    QVBoxLayout* viceLayout = new QVBoxLayout();
    QLabel* imgVice = new QLabel(this);
    imgVice->setPixmap(QPixmap("assets/cities/Vice City.png").scaled(180, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imgVice->setAlignment(Qt::AlignCenter);
    QLabel* lblVice = new QLabel("Vice City", this);
    lblVice->setAlignment(Qt::AlignCenter);
    QPushButton* btnVice = new QPushButton("Elegir", this);
    connect(btnVice, &QPushButton::clicked, this, [this]() { on_map_selected("ViceCity"); });
    viceLayout->addWidget(imgVice);
    viceLayout->addWidget(lblVice);
    viceLayout->addWidget(btnVice);

    mapsLayout->addLayout(libertyLayout);
    mapsLayout->addSpacing(30);
    mapsLayout->addLayout(sanLayout);
    mapsLayout->addSpacing(30);
    mapsLayout->addLayout(viceLayout);

    mainLayout->addLayout(mapsLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

void SelectionMapScreen::on_map_selected(const QString& map_name) {
    selected_map = map_name; // Guardar selección
    emit go_to_waiting_room_screen();
}