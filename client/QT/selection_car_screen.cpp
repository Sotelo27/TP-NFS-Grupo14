#include "selection_car_screen.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QIcon>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QLabel> // agregado
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
    QLabel* title = new QLabel("Seleccioná tu auto", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 24px; font-weight: 800; letter-spacing: 1px;"
        "color: #FF00C8;"
        "padding: 6px 10px; border-bottom: 2px solid rgba(255,0,200,0.35);"
    );
    mainLayout->addWidget(title);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setContentsMargins(20, 10, 20, 10);
    gridLayout->setHorizontalSpacing(16);
    gridLayout->setVerticalSpacing(16);
    mainLayout->addLayout(gridLayout);

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
            "  background-color: rgba(10,0,25,0.60);"
            "  border: 2px solid #7D00FF;"
            "  border-radius: 12px;"
            "}"
            "QPushButton:hover {"
            "  border-color: #FF00C8;"
            "}"
            "QPushButton:checked {"
            "  border: 3px solid #00FFE2;"
            "  background-color: rgba(20,0,45,0.70);"
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
    // Estilo vaporwave para botón primario
    listoBtn->setStyleSheet(
        "QPushButton {"
        "  font-size: 18px; font-weight: 800; letter-spacing: 2px;"
        "  color: #0afff7;"
        "  padding: 12px 26px;"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #7300FF, stop:1 #FF00C8);"
        "  border: 3px solid rgba(255,255,255,0.35);"
        "  border-radius: 14px;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #FF00C8, stop:1 #00FFE2);"
        "}"
        "QPushButton:pressed {"
        "  background:#280040;"
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