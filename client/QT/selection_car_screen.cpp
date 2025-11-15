#include "selection_car_screen.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QSizePolicy>

SelectionCarScreen::SelectionCarScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    // Fondo full screen
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap("assets/images/garage.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();

    // Layout principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 30);
    mainLayout->setSpacing(10);

    // Empuja todo hacia abajo
    mainLayout->addStretch();

    // ------------------------------
    // AUTO GRANDE
    // ------------------------------
    carLabel = new QLabel();
    carLabel->setAlignment(Qt::AlignCenter);
    carLabel->setFixedSize(750, 390);   // más grande que antes

    // Añadimos un pequeño espacio para bajarlo más
    mainLayout->addSpacing(40);
    mainLayout->addWidget(carLabel, 0, Qt::AlignCenter);

    // ------------------------------
    // FLECHAS + LISTO
    // ------------------------------
    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(25);

    QPushButton* leftBtn = new QPushButton("<");
    leftBtn->setFixedSize(70, 70);
    leftBtn->setStyleSheet(
        "background-color: rgba(0, 0, 0, 100);"
        "color: white;"
        "border-radius: 10px;"
        "font-size: 28px;"
    );

    QPushButton* listoBtn = new QPushButton("Listo");
    listoBtn->setFixedSize(200, 55);
    listoBtn->setStyleSheet(
        "background-color: rgba(0, 0, 0, 100);"
        "color: white;"
        "border-radius: 10px;"
        "font-size: 20px;"
    );

    QPushButton* rightBtn = new QPushButton(">");
    rightBtn->setFixedSize(70, 70);
    rightBtn->setStyleSheet(
        "background-color: rgba(0, 0, 0, 100);"
        "color: white;"
        "border-radius: 10px;"
        "font-size: 28px;"
    );

    bottomRow->addWidget(leftBtn);
    bottomRow->addWidget(listoBtn, 0, Qt::AlignCenter);
    bottomRow->addWidget(rightBtn);

    mainLayout->addLayout(bottomRow);

    // Lista de autos
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

    // Signals
    connect(leftBtn,  &QPushButton::clicked, this, &SelectionCarScreen::prevCar);
    connect(rightBtn, &QPushButton::clicked, this, &SelectionCarScreen::nextCar);

    connect(listoBtn, &QPushButton::clicked, this, [this]() {
        emit car_selected(cars[currentIndex].id);
        emit go_to_menu();
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
    backgroundLabel->setGeometry(0, 0, width(), height());

    int w = width();
    int newWidth = qBound(600, w * 75 / 100, 1200);
    carLabel->setFixedSize(newWidth, newWidth * 0.52);

    updateCarImage();
}

void SelectionCarScreen::setSelectedCarIndex(int idx) {
    if (idx >= 0 && idx < cars.size()) {
        currentIndex = idx;
        updateCarImage();
    }
}

int SelectionCarScreen::getSelectedCarIndex() const {
    return currentIndex;
}

CarSpriteID SelectionCarScreen::getSelectedCarId() const {
    return cars[currentIndex].id;
}

int SelectionCarScreen::findCarIndexById(CarSpriteID id) const {
    for (int i = 0; i < cars.size(); ++i) {
        if (cars[i].id == id) return i;
    }
    return 0; // por defecto
}

