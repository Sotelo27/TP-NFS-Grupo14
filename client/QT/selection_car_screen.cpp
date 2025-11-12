#include "selection_car_screen.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QLabel> 
#include "../../common/enum/car_enum.h"

struct CarInfoSprite {
    CarSpriteID id;
    QString imagePath;
};

SelectionCarScreen::SelectionCarScreen(ServerHandler& server_handler, QWidget* parent)
    : QWidget(parent), server_handler(server_handler)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Título estilizado (sin fondo de imagen)
    QLabel* title = new QLabel("Seleccionar auto", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 22px; font-weight: 700; letter-spacing: 0.5px;"
        "color: #B27CE8;"
        "padding: 8px 6px; background-color: rgba(255,255,255,0.35);"
        "border: 1px solid rgba(178,124,232,0.4); border-radius: 10px;"
    );
    mainLayout->addWidget(title);

    // Contenedor suave para la grilla
    QWidget* gridFrame = new QWidget(this);
    gridFrame->setStyleSheet(
        "background-color: rgba(240,240,245,0.55);"
        "border: 1px solid rgba(178,124,232,0.35);"
        "border-radius: 12px; padding: 12px;"
    );
    QVBoxLayout* frameLayout = new QVBoxLayout(gridFrame);
    frameLayout->setContentsMargins(4,4,4,4);
    frameLayout->setSpacing(8);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(10, 6, 10, 6);
    gridLayout->setHorizontalSpacing(14);
    gridLayout->setVerticalSpacing(14);
    frameLayout->addLayout(gridLayout);
    mainLayout->addWidget(gridFrame);

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    if (!QFile::exists("assets/cars/cars_images/limusina.jpg")) {
        std::cout << "El archivo no existe en la ruta:" << std::endl;
    }
    QVector<CarInfoSprite> cars = {
        {CarSpriteID::CommonGreenCar, "assets/cars/cars_images/autoVerde.png"},
        {CarSpriteID::RedCar, "assets/cars/cars_images/autoRojoDeportivo.png"},
        {CarSpriteID::RedSportsCar, "assets/cars/cars_images/autoPorcheRojo.png"},
        {CarSpriteID::SpecialCar, "assets/cars/cars_images/autoAzulClaro.png"},
        {CarSpriteID::FourByFourConvertible, "assets/cars/cars_images/jeepRojo.png"},
        {CarSpriteID::PickupTruck, "assets/cars/cars_images/camionetaAzul.png"},
        {CarSpriteID::Limousine, "assets/cars/cars_images/limusina.png"}
    };

    int row = 0, col = 0;
    for (int i = 0; i < cars.size(); ++i) {
        QPushButton* btn = new QPushButton(this);
        btn->setCheckable(true);
        btn->setIcon(QIcon(cars[i].imagePath));
        btn->setIconSize(QSize(200, 120));
        btn->setFixedSize(220, 140);
        // Estilo vaporwave para cada tarjeta de auto
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: rgba(255,255,255,0.70);"
            "  border: 2px solid rgba(178,124,232,0.55);"
            "  border-radius: 12px;"
            "}"
            "QPushButton:hover {"
            "  border-color: rgba(255,159,217,0.75);"
            "  background-color: rgba(255,255,255,0.85);"
            "}"
            "QPushButton:checked {"
            "  border: 3px solid rgba(120,230,224,0.85);"
            "  background-color: rgba(255,255,255,0.95);"
            "}"
        );

        gridLayout->addWidget(btn, row, col);
        buttonGroup->addButton(btn, i);

        if (++col >= 3) {
            col = 0;
            ++row;
        }
    }

    mainLayout->addSpacing(12);

    QPushButton* listoBtn = new QPushButton("Listo", this);
    // Estilo para boton primario
    listoBtn->setStyleSheet(
        "QPushButton {"
        "  font-size: 16px; font-weight: 700; letter-spacing: 1px;"
        "  color: #3B3B44;"
        "  padding: 10px 24px;"
        "  background: linear-gradient(135deg, rgba(255,159,217,0.65), rgba(178,124,232,0.65));"
        "  border: 2px solid rgba(178,124,232,0.50);"
        "  border-radius: 12px;"
        "}"
        "QPushButton:hover {"
        "  background: linear-gradient(135deg, rgba(255,159,217,0.80), rgba(120,230,224,0.70));"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(178,124,232,0.55);"
        "}"
    );
    mainLayout->addWidget(listoBtn, 0, Qt::AlignCenter);

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