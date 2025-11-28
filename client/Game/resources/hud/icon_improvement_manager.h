#ifndef ICON_IMPROVEMENT_MANAGER_H
#define ICON_IMPROVEMENT_MANAGER_H

#include <unordered_map>

#include "../../../../common/enum/car_improvement.h"
#include "../../sdl_wrappers/SdlObjTexture.h"
#include "../../sdl_wrappers/SdlWindow.h"

class IconImprovementManager {
private:
    std::unordered_map<CarImprovement, SdlObjTexture&> icons;
    SdlObjTexture icon_controllability;
    SdlObjTexture icon_health;
    SdlObjTexture icon_speed;
    SdlObjTexture icon_acceleration;
    SdlObjTexture icon_mass;

public:
    explicit IconImprovementManager(const SdlWindow& window);

    SdlObjTexture& get_icon(CarImprovement improvement);
};

#endif  // ICON_IMPROVEMENT_MANAGER_H
