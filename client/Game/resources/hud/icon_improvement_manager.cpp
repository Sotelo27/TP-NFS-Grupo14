#include "icon_improvement_manager.h"

#include "../../constants.h"

#define ICON_CONTROLLABILITY_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/control.png"
#define ICON_HEALTH_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/health.png"
#define ICON_SPEED_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/speed.png"
#define ICON_ACCELERATION_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/acceleration.png"
#define ICON_MASS_IMAGE_PATH std::string(ASSETS_PATH) + "/mid/mass.png"

IconImprovementManager::IconImprovementManager(const SdlWindow& window):
        icons(),
        icon_controllability(ICON_CONTROLLABILITY_IMAGE_PATH, window, RGB_BACKGROUND),
        icon_health(ICON_HEALTH_IMAGE_PATH, window, RGB_BACKGROUND),
        icon_speed(ICON_SPEED_IMAGE_PATH, window, RGB_BACKGROUND),
        icon_acceleration(ICON_ACCELERATION_IMAGE_PATH, window, RGB_BACKGROUND),
        icon_mass(ICON_MASS_IMAGE_PATH, window, RGB_BACKGROUND) {
    icons.emplace(CarImprovement::Controllability, icon_controllability);
    icons.emplace(CarImprovement::Health, icon_health);
    icons.emplace(CarImprovement::Speed, icon_speed);
    icons.emplace(CarImprovement::Acceleration, icon_acceleration);
    icons.emplace(CarImprovement::Mass, icon_mass);
}

SdlObjTexture& IconImprovementManager::get_icon(CarImprovement improvement) {
    return icons.at(improvement);
}
