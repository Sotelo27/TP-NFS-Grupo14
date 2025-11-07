#ifndef CAR_SPRITE_SHEET_H
#define CAR_SPRITE_SHEET_H

#include <string>
#include <unordered_map>

#include "../../common/enum/car_enum.h"
#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../utils/Area.h"

struct CarData {
    Area area;
    int width_scale_screen;
    int height_scale_screen;
};

class CarSpriteSheet {
private:
    SdlObjTexture texture_cars;
    std::unordered_map<CarSpriteID, CarData> sprites;

public:
    explicit CarSpriteSheet(const SdlWindow& window);

    const CarData& getCarData(CarSpriteID id) const;
    void render(const Area& src, const Area& dest, float angle) const;
};

#endif  // CAR_SPRITE_SHEET_H
