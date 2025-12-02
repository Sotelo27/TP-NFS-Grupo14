#include "selection_car_screen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>

SelectionCarScreen::SelectionCarScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    setupUi();
    setupConnections();
}

void SelectionCarScreen::setupUi() {
    createBackground();
    createCarLabel();
    createButtons();

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 20, 30, 30);
    mainLayout->setSpacing(10);
    mainLayout->addStretch();
    mainLayout->addSpacing(40);
    mainLayout->addWidget(carLabel, 0, Qt::AlignCenter);

    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(25);
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
}

void SelectionCarScreen::createBackground() {
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap("assets/images/garage.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->lower();
}

void SelectionCarScreen::createCarLabel() {
    carLabel = new QLabel();
    carLabel->setAlignment(Qt::AlignCenter);
    carLabel->setFixedSize(900, 540);
}

void SelectionCarScreen::createButtons() {
    leftBtn = new QPushButton("<");
    leftBtn->setFixedSize(70, 70);
    leftBtn->setStyleSheet(
        "background-color: rgba(0,0,0,100); color:white; border-radius:10px; font-size:28px;"
    );

    rightBtn = new QPushButton(">");
    rightBtn->setFixedSize(70, 70);
    rightBtn->setStyleSheet(leftBtn->styleSheet());

    listoBtn = new QPushButton("Listo");
    listoBtn->setFixedSize(200, 55);
    listoBtn->setStyleSheet(
        "background-color: rgba(0,0,0,100); color:white; border-radius:10px; font-size:20px;"
    );
}

void SelectionCarScreen::setupConnections() {
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
    return 0;
}

// ------------------
// Ajuste del fondo al tamaño del widget
// ------------------
void SelectionCarScreen::resizeEvent(QResizeEvent* event) {
    if (backgroundLabel) {
        backgroundLabel->setGeometry(0, 0, width(), height());
    }
    QWidget::resizeEvent(event);
}
