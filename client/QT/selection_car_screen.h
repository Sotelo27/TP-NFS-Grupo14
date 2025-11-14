#ifndef SELECTION_CAR_SCREEN_H
#define SELECTION_CAR_SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QVector>

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

signals:
    void go_to_lobby();
    void car_selected(CarSpriteID id);

private slots:
    void nextCar();
    void prevCar();
    void updateCarImage();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    ServerHandler& server_handler;

    QLabel* backgroundLabel;
    QLabel* carLabel;

    QVector<CarInfoSprite> cars;
    int currentIndex = 0;
};

#endif
