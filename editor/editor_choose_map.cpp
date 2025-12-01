#include "editor_choose_map.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPixmap>
#include <QDebug>

EditorChooseMap::EditorChooseMap(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Seleccionar Mapa - Need For Speed");
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    // Fondo general de la ventana
    background = new QLabel(this);
    background->setPixmap(QPixmap("assets/images/fondo.png"));
    background->setScaledContents(true);
    background->setGeometry(0, 0, width(), height());
    background->lower(); // queda detrás de todos los widgets

    showMaximized(); // ahora que el fondo está creado

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(18);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    QLabel* title = new QLabel("SELECCIONAR MAPA", this);
    title->setFixedHeight(100);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "background-color: rgba(255, 0, 180, 0.20);"
        "border: 5px solid #ff33cc;"
        "border-radius: 18px;"
        "font-size: 48px;"
        "font-weight: 900;"
        "color: #ffccff;"
    );
    auto* titleGlow = new QGraphicsDropShadowEffect(this);
    titleGlow->setBlurRadius(55);
    titleGlow->setOffset(0, 0);
    titleGlow->setColor(QColor(255, 0, 180, 200));
    title->setGraphicsEffect(titleGlow);
    mainLayout->addWidget(title, 0, Qt::AlignHCenter);

    QWidget* panel = new QWidget(this);
    panel->setStyleSheet(
        "background: rgba(15, 5, 25, 0.85);"
        "border-radius: 20px;"
        "border: 3px solid rgba(255, 0, 180, 0.18);"
    );
    auto* panelGlow = new QGraphicsDropShadowEffect(panel);
    panelGlow->setBlurRadius(60);
    panelGlow->setOffset(0, 0);
    panelGlow->setColor(QColor(255, 0, 180, 120));
    panel->setGraphicsEffect(panelGlow);

    QVBoxLayout* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(36, 26, 36, 26);
    panelLayout->setSpacing(22);
    panelLayout->setAlignment(Qt::AlignTop);

    QHBoxLayout* mapsLayout = new QHBoxLayout();
    mapsLayout->setSpacing(50);
    mapsLayout->setContentsMargins(8, 8, 8, 8);
    mapsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    auto createMapCard = [&](const QString& imgPath,
                             const QString& mapLabel,
                             const QString& internalName)
    {
        QWidget* cardWidget = new QWidget(panel);
        cardWidget->setFixedSize(350, 410);
        cardWidget->setStyleSheet(
            "background: rgba(255,255,255,0.04);"
            "border-radius: 16px;"
            "border: 2px solid rgba(200,120,255,0.20);"
        );
        auto* cardGlow = new QGraphicsDropShadowEffect(cardWidget);
        cardGlow->setBlurRadius(32);
        cardGlow->setOffset(0, 0);
        cardGlow->setColor(QColor(180, 100, 255, 120));
        cardWidget->setGraphicsEffect(cardGlow);

        QVBoxLayout* cardLayout = new QVBoxLayout(cardWidget);
        cardLayout->setContentsMargins(12, 12, 12, 12);
        cardLayout->setSpacing(10);
        cardLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

        QWidget* imgBox = new QWidget(cardWidget);
        imgBox->setFixedSize(300, 240);
        imgBox->setStyleSheet(
            "background: rgba(10,0,20,0.55);"
            "border-radius: 12px;"
            "border: 1px solid rgba(180,120,255,0.25);"
        );
        QVBoxLayout* imgBoxLayout = new QVBoxLayout(imgBox);
        imgBoxLayout->setContentsMargins(8, 8, 8, 8);

        QLabel* imgLabel = new QLabel(imgBox);
        QPixmap pm(imgPath);
        imgLabel->setPixmap(pm.scaled(imgBox->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imgLabel->setAlignment(Qt::AlignCenter);
        imgBoxLayout->addWidget(imgLabel, 0, Qt::AlignCenter);

        QLabel* nameLabel = new QLabel(mapLabel, cardWidget);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setFixedHeight(34);
        nameLabel->setStyleSheet(
            "background: rgba(0,0,0,0.55);"
            "color: #ffdfff;"
            "font-weight: 800;"
            "font-size: 18px;"
            "border-radius: 6px;"
            "border: 1px solid rgba(150,100,255,0.35);"
        );

        QPushButton* chooseBtn = new QPushButton("Elegir", cardWidget);
        chooseBtn->setCursor(Qt::PointingHandCursor);
        chooseBtn->setFixedSize(120, 40);
        chooseBtn->setStyleSheet(
            "QPushButton {"
            "font-size: 16px; font-weight: 800;"
            "color: #1A1A22;"
            "background: linear-gradient(135deg, #FF9FD9, #B27CE8);"
            "border-radius: 10px;"
            "border: 2px solid rgba(178,124,232,0.85);"
            "}"
            "QPushButton:hover {"
            "background: linear-gradient(135deg, #FFB3E4, #8AF5F0);"
            "}"
            "QPushButton:pressed { background: rgba(178,124,232,0.6); }"
        );

        connect(chooseBtn, &QPushButton::clicked, this,
                [this, internalName, imgPath]() {
            on_map_selected(internalName, imgPath);
        });

        cardLayout->addWidget(imgBox, 0, Qt::AlignCenter);
        cardLayout->addSpacing(6);
        cardLayout->addWidget(nameLabel);
        cardLayout->addSpacing(12);
        cardLayout->addWidget(chooseBtn, 0, Qt::AlignCenter);

        return cardWidget;
    };

    mapsLayout->addWidget(createMapCard("assets/cities/LibertyCity.png",  "Liberty City",  "LibertyCity"));
    mapsLayout->addWidget(createMapCard("assets/cities/SanAndreas.png",  "San Andreas",  "SanAndreas"));
    mapsLayout->addWidget(createMapCard("assets/cities/ViceCity.png",    "Vice City",    "ViceCity"));

    panelLayout->addLayout(mapsLayout);
    mainLayout->addWidget(panel, 0, Qt::AlignHCenter);
    mainLayout->addStretch();
}

void EditorChooseMap::on_map_selected(const QString& map_name, const QString& img_path) {
    selected_map = map_name;
    selected_map_image = img_path;
    emit go_to_editor_screen();
}

void EditorChooseMap::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (background)
        background->setGeometry(0, 0, width(), height());
}
