#ifndef CAR_SPRITE_SHEET_H
#define CAR_SPRITE_SHEET_H

#include <string>
#include <unordered_map>

#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"

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
    SdlObjTexture texture_cars;
    std::unordered_map<CarSpriteID, CarData> sprites;

public:
    explicit CarSpriteSheet(const SdlWindow& window);

    const CarData& getCarData(CarSpriteID id) const;
    void render(const Area& src, const Area& dest) const;
    void render_rotated(const Area& src, const Area& dest, double angle_deg) const;
};

#endif  // CAR_SPRITE_SHEET_H
