#ifndef SPEED_HUD_H
#define SPEED_HUD_H

#include <string>
#include <unordered_map>

#include "../sdl_wrappers/SdlFont.h"
#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../utils/Area.h"

class SpeedHud {
private:
    SdlObjTexture texture;
    SdlFont textNumber;
    SdlFont textUnit;

public:
    explicit SpeedHud(const SdlWindow& window);

    void render(int speed, int x, int y);
};

#endif  // SPEED_HUD_H
