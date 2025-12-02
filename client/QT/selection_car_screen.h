#ifndef SELECTION_CAR_SCREEN_H
#define SELECTION_CAR_SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QResizeEvent>

#include "../connection/server_handler.h"
#include "../../common/enum/car_enum.h"

struct CarInfoSprite {
    CarSpriteID id;
    QString imagePath;
};

class SelectionCarScreen : public QWidget {
    Q_OBJECT

public:
    explicit SelectionCarScreen(ServerHandler& server_handler, QWidget* parent = nullptr);

    void setSelectedCarIndex(int idx);
    int getSelectedCarIndex() const;
    CarSpriteID getSelectedCarId() const;
    int findCarIndexById(CarSpriteID id) const;

    signals:
        void car_selected(CarSpriteID car_id);
    void go_to_menu();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void nextCar();
    void prevCar();
    void updateCarImage();

private:
    ServerHandler& server_handler;

    QLabel* backgroundLabel = nullptr;
    QLabel* carLabel = nullptr;

    QPushButton* leftBtn = nullptr;
    QPushButton* rightBtn = nullptr;
    QPushButton* listoBtn = nullptr;

    QVector<CarInfoSprite> cars;
    int currentIndex = 0;

    void setupUi();
    void createBackground();
    void createCarLabel();
    void createButtons();
    void setupConnections();
};

#endif // SELECTION_CAR_SCREEN_H
