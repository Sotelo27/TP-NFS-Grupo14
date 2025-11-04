#include "car_sprite_sheet.h"

#include <utility>

#include "../constants.h"

#define CAR_SPRITE_SHEET_FILE std::string(ASSETS_PATH) + "/cars/Cars.png"

CarSpriteSheet::CarSpriteSheet(const SdlWindow& window):
        texture_cars(CAR_SPRITE_SHEET_FILE, window,
                     Rgb(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B)) {
    sprites.emplace(CarSpriteID::CommonGreenCar,
                    CarData{Area(0, 0, 32, 32), CAR_WIDTH_SMALL, CAR_HEIGHT_SMALL});
    sprites.emplace(CarSpriteID::RedCar,
                    CarData{Area(0, 64, 40, 40), CAR_WIDTH_MEDIUM, CAR_HEIGHT_MEDIUM});
    sprites.emplace(CarSpriteID::RedSportsCar,
                    CarData{Area(0, 144, 40, 40), CAR_WIDTH_MEDIUM, CAR_HEIGHT_MEDIUM});
    sprites.emplace(CarSpriteID::SpecialCar,
                    CarData{Area(0, 224, 40, 40), CAR_WIDTH_MEDIUM, CAR_HEIGHT_MEDIUM});
    sprites.emplace(CarSpriteID::FourByFourConvertible,
                    CarData{Area(0, 304, 40, 40), CAR_WIDTH_MEDIUM, CAR_HEIGHT_MEDIUM});
    sprites.emplace(CarSpriteID::PickupTruck,
                    CarData{Area(0, 384, 40, 40), CAR_WIDTH_MEDIUM, CAR_HEIGHT_MEDIUM});
    sprites.emplace(CarSpriteID::Limousine,
                    CarData{Area(0, 464, 48, 48), CAR_WIDTH_LARGE, CAR_HEIGHT_LARGE});
}

const CarData& CarSpriteSheet::getCarData(CarSpriteID id) const { return this->sprites.at(id); }

void CarSpriteSheet::render(const Area& src, const Area& dest) const {
    this->texture_cars.render(src, dest);
}

void CarSpriteSheet::render_rotated(const Area& src, const Area& dest, double angle_deg) const {
    this->texture_cars.render_rotated(src, dest, angle_deg);
}
