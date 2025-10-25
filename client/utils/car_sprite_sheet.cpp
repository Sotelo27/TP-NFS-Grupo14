#include "car_sprite_sheet.h"

#include <utility>

#include "Area.h"

CarSpriteSheet::CarSpriteSheet() {
    sprites.emplace(CarSpriteID::CommonGreenCar, Area(0, 0, 32, 32));
    sprites.emplace(CarSpriteID::RedCar, Area(0, 64, 40, 40));
    sprites.emplace(CarSpriteID::RedSportsCar, Area(0, 144, 40, 40));
    sprites.emplace(CarSpriteID::SpecialCar, Area(0, 224, 40, 40));
    sprites.emplace(CarSpriteID::FourByFourConvertible, Area(0, 304, 40, 40));
    sprites.emplace(CarSpriteID::PickupTruck, Area(0, 384, 40, 40));
    sprites.emplace(CarSpriteID::Limousine, Area(0, 464, 48, 48));
}

const Area& CarSpriteSheet::get(CarSpriteID id) const { return this->sprites.at(id); }
