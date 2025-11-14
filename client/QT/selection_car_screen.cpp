#include "selection_car_screen.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QDebug>
#include <QSizePolicy>

SelectionCarScreen::SelectionCarScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    // -------------------------
    // BACKGROUND fullscreen
    // -------------------------
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap("assets/images/garage.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
    backgroundLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    // -------------------------
    // MAIN LAYOUT
    // -------------------------
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 30);
    mainLayout->setSpacing(10);

    mainLayout->addStretch(); // empuja todo hacia abajo

    // -------------------------
    // FLECHA - AUTO - FLECHA
    // -------------------------
    QHBoxLayout* middleLayout = new QHBoxLayout();
    middleLayout->setSpacing(25);

    QPushButton* leftBtn = new QPushButton("<");
    leftBtn->setFixedSize(65, 65);
    leftBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(255,255,255,0.65);"
        "  border: 2px solid #9ED0FF;"
        "  border-radius: 14px;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(255,255,255,0.85); }"
    );

    carLabel = new QLabel();
    carLabel->setFixedSize(520, 260);
    carLabel->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    carLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton* rightBtn = new QPushButton(">");
    rightBtn->setFixedSize(65, 65);
    rightBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(255,255,255,0.65);"
        "  border: 2px solid #9ED0FF;"
        "  border-radius: 14px;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(255,255,255,0.85); }"
    );

    middleLayout->addWidget(leftBtn, 0, Qt::AlignBottom);
    middleLayout->addWidget(carLabel, 1, Qt::AlignBottom);
    middleLayout->addWidget(rightBtn, 0, Qt::AlignBottom);

    mainLayout->addLayout(middleLayout);

    // -------------------------
    // BOTÓN LISTO
    // -------------------------
    QPushButton* listoBtn = new QPushButton("Listo");
    listoBtn->setFixedSize(170, 52);
    listoBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255,255,255,0.30);"
        "  font-size: 19px;"
        "  font-weight: bold;"
        "  border: 2px solid #FF84C6;"
        "  border-radius: 16px;"
        "}"
        "QPushButton:hover { background: rgba(255,210,244,0.5); }"
    );

    mainLayout->addWidget(listoBtn, 0, Qt::AlignCenter);
    mainLayout->addStretch();

    // -------------------------
    // LISTA DE AUTOS
    // -------------------------
    cars = {
        {CarSpriteID::CommonGreenCar, "assets/cars/cars_images/autoVerde.png"},
        {CarSpriteID::RedCar, "assets/cars/cars_images/autoRojoDeportivo.png"},
        {CarSpriteID::RedSportsCar, "assets/cars/cars_images/autoPorcheRojo.png"},
        {CarSpriteID::SpecialCar, "assets/cars/cars_images/autoAzulClaro.png"},
        {CarSpriteID::FourByFourConvertible, "assets/cars/cars_images/jeepRojo.png"},
        {CarSpriteID::PickupTruck, "assets/cars/cars_images/camionetaAzul.png"},
        {CarSpriteID::Limousine, "assets/cars/cars_images/limusina.png"}
    };

    updateCarImage();

    // -------------------------
    // SIGNALS
    // -------------------------
    connect(leftBtn,  &QPushButton::clicked, this, &SelectionCarScreen::prevCar);
    connect(rightBtn, &QPushButton::clicked, this, &SelectionCarScreen::nextCar);

    connect(listoBtn, &QPushButton::clicked, [this]() {
        emit car_selected(cars[currentIndex].id);
        emit go_to_lobby();
    });
}

void SelectionCarScreen::prevCar() {
    currentIndex = (currentIndex - 1 + cars.size()) % cars.size();
    updateCarImage();
}

void SelectionCarScreen::nextCar() {
    currentIndex = (currentIndex + 1) % cars.size();
    updateCarImage();
}

void SelectionCarScreen::updateCarImage() {
    QPixmap px(cars[currentIndex].imagePath);
    carLabel->setPixmap(px.scaled(carLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void SelectionCarScreen::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (backgroundLabel)
        backgroundLabel->setGeometry(0, 0, width(), height());

    // auto más grande en pantallas grandes
    int w = width();
    int maxW = qBound(340, w * 42 / 100, 720);

    carLabel->setFixedSize(maxW, maxW * 0.52);
    updateCarImage();
}
