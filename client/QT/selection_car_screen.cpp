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
    // ------------------------------------
    // Fondo del garage (pantalla completa)
    // ------------------------------------
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap("assets/images/garage.png"));
    backgroundLabel->setScaledContents(true);
    backgroundLabel->setGeometry(0, 0, width(), height());
    backgroundLabel->lower();
    backgroundLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    // ------------------------------------
    // Layout principal
    // ------------------------------------
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // Ajusta márgenes para que el auto se ubique más abajo en la imagen de fondo
    mainLayout->setContentsMargins(24, 8, 24, 18);
    mainLayout->setSpacing(8);

    // Añadimos un stretch arriba para empujar el contenido hacia abajo (más "cerca del suelo")
    mainLayout->addStretch(3);

    // -----------------------------
    // Auto (centrado, más abajo)
    // -----------------------------
    carLabel = new QLabel();
    // tamaño razonable para que el auto no ocupe TODO el fondo
    carLabel->setFixedSize(520, 300);
    carLabel->setAlignment(Qt::AlignCenter);
    carLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Centrar el carLabel horizontalmente
    mainLayout->addWidget(carLabel, 0, Qt::AlignHCenter | Qt::AlignVCenter);

    // -----------------------------
    // Flechas pequeñas debajo del auto
    // -----------------------------
    QHBoxLayout* arrowsLayout = new QHBoxLayout();
    arrowsLayout->setContentsMargins(0, 6, 0, 6);
    arrowsLayout->setSpacing(18);

    QPushButton* leftBtn = new QPushButton("<");
    // flechas MÁS pequeñas y con menor "huella" en el fondo
    leftBtn->setFixedSize(56, 56);
    leftBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0, x2:1,y2:1, stop:0 #FFFFFF, stop:1 #C6E4FF );"
        "  border: 2px solid #9ED0FF;"
        "  border-radius: 12px;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "  color: #223;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(215,178,255,0.8);"
        "  border-color: #E080FF;"
        "  color: #000;"
        "}"
    );

    QPushButton* rightBtn = new QPushButton(">");
    rightBtn->setFixedSize(56, 56);
    rightBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0, x2:1,y2:1, stop:0 #FFFFFF, stop:1 #C6E4FF );"
        "  border: 2px solid #9ED0FF;"
        "  border-radius: 12px;"
        "  font-size: 28px;"
        "  font-weight: bold;"
        "  color: #223;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(215,178,255,0.8);"
        "  border-color: #E080FF;"
        "  color: #000;"
        "}"
    );

    // Spacer para que las flechas no queden pegadas al centro si querés más separación
    arrowsLayout->addStretch(1);
    arrowsLayout->addWidget(leftBtn, 0, Qt::AlignCenter);
    arrowsLayout->addSpacing(18);
    arrowsLayout->addWidget(rightBtn, 0, Qt::AlignCenter);
    arrowsLayout->addStretch(1);

    mainLayout->addLayout(arrowsLayout);

    // -----------------------------
    // Botón "Listo" centrado abajo
    // -----------------------------
    QPushButton* listoBtn = new QPushButton("Listo");
    listoBtn->setFixedSize(160, 48);
    listoBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255,255,255,0.30);"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  border: 2px solid #FF84C6;"
        "  border-radius: 14px;"
        "  padding: 6px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255,210,244,0.5);"
        "}"
    );

    // Centrar "Listo"
    mainLayout->addWidget(listoBtn, 0, Qt::AlignHCenter);

    // Añadimos un stretch final para empujar todo ligeramente hacia arriba si hace resize
    mainLayout->addStretch(2);

    // -----------------------------
    // Lista de autos (igual que antes)
    // -----------------------------
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

    // -----------------------------
    // Señales (misma lógica)
    // -----------------------------
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
    if (cars.isEmpty() || carLabel == nullptr) return;
    QPixmap px(cars[currentIndex].imagePath);
    carLabel->setPixmap(px.scaled(carLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void SelectionCarScreen::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (backgroundLabel) {
        backgroundLabel->setGeometry(0, 0, width(), height());
    }
    // Opcional: ajustar tamaño del carLabel en función del ancho de la ventana para mantener proporciones
    int w = width();
    int newCarW = qBound(320, w * 45 / 100, 700); // entre 320 y 700 px, relativo al ancho
    int newCarH = newCarW * 9 / 16; // aproximación 16:9
    carLabel->setFixedSize(newCarW, newCarH);
    updateCarImage();
}
