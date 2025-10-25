#ifndef CAR_SPRITE_SHEET_H
#define CAR_SPRITE_SHEET_H

#include <string>
#include <unordered_map>

class Area;

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
    std::unordered_map<CarSpriteID, Area> sprites;

public:
    CarSpriteSheet();

    const Area& get(CarSpriteID id) const;
};

#endif  // CAR_SPRITE_SHEET_H
