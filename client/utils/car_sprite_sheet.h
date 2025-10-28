#ifndef CAR_SPRITE_SHEET_H
#define CAR_SPRITE_SHEET_H

#include <string>
#include <unordered_map>

#include "Area.h"
struct CarData {
    Area area;
    int width_scale_screen;
    int height_scale_screen;
};

enum class CarSpriteID {
    CommonGreenCar,
    RedCar,
    RedSportsCar,
    SpecialCar,
    FourByFourConvertible,
    PickupTruck,
    Limousine
};

class CarSpriteSheet {
private:
    std::unordered_map<CarSpriteID, CarData> sprites;

public:
    CarSpriteSheet();

    const Area& getArea(CarSpriteID id) const;
    const CarData& get(CarSpriteID id) const;
};

#endif  // CAR_SPRITE_SHEET_H
