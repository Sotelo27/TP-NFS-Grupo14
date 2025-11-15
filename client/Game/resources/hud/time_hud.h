#ifndef TIME_HUD_H
#define TIME_HUD_H

#include <string>
#include <unordered_map>

#include "../../sdl_wrappers/SdlFont.h"
#include "../../sdl_wrappers/SdlObjTexture.h"
#include "../../sdl_wrappers/SdlWindow.h"
#include "../../utils/Area.h"

class TimeHud {
private:
    SdlObjTexture texture;
    SdlFont text;

public:
    explicit TimeHud(const SdlWindow& window);

    void render(int16_t time_seconds, int x, int y);
};

#endif  // TIME_HUD_H
