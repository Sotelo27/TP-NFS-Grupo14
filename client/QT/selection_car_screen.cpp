#include "selection_car_screen.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include "../../common/enum/car_enum.h"

struct CarInfoSprite {
    CarSpriteID id;
    QString imagePath;
};

SelectionCarScreen::SelectionCarScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QGridLayout* gridLayout = new QGridLayout();
    mainLayout->addLayout(gridLayout);

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    if (!QFile::exists("assets/cars/cars_images/limusina.jpg")) {
        std::cout << "El archivo no existe en la ruta:" << std::endl;
    }
    QVector<CarInfoSprite> cars = {
        {CarSpriteID::CommonGreenCar, "assets/cars/cars_images/autoVerde.webp"},
        {CarSpriteID::RedCar, "assets/cars/cars_images/autoRojoDeportivo.jpg"},
        {CarSpriteID::RedSportsCar, "assets/cars/cars_images/autoTojoPorche.jpeg"},
        {CarSpriteID::SpecialCar, ":assets/cars/cars_images/autoAzulClari.jpg"},
        {CarSpriteID::FourByFourConvertible, "assets/cars/cars_images/jeepRojo.jpg"},
        {CarSpriteID::PickupTruck, ":assets/cars/cars_images/camionetaAzul.avif"},
        {CarSpriteID::Limousine, "assets/cars/cars_images/limusina.jpg"}
    };

    int row = 0, col = 0;
    for (int i = 0; i < cars.size(); ++i) {
        QPushButton* btn = new QPushButton(this);
        btn->setCheckable(true);
        btn->setIcon(QIcon(cars[i].imagePath));
        btn->setIconSize(QSize(200, 120));

        gridLayout->addWidget(btn, row, col);
        buttonGroup->addButton(btn, i);

        if (++col >= 3) {
            col = 0;
            ++row;
        }
    }

    QPushButton* listoBtn = new QPushButton("Listo", this);
    mainLayout->addWidget(listoBtn);

    connect(listoBtn, &QPushButton::clicked, [this, buttonGroup, cars]() {
        int id = buttonGroup->checkedId();
        if (id >= 0 && id < cars.size()) {
            CarSpriteID selectedCar = cars[id].id;
            qDebug() << "Auto seleccionado:" << static_cast<int>(selectedCar);

            emit car_selected(selectedCar);

            emit go_to_lobby();
        } else {
            qDebug() << "No se seleccionó ningún auto";
        }
    });
}