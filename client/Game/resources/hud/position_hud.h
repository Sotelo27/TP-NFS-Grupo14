#ifndef POSITION_HUD_H
#define POSITION_HUD_H

#include <string>
#include <unordered_map>

#include "../../sdl_wrappers/SdlFont.h"
#include "../../sdl_wrappers/SdlObjTexture.h"
#include "../../sdl_wrappers/SdlWindow.h"
#include "../../utils/Area.h"

class PositionHud {
private:
    SdlFont text;

    std::string getOrdinalSuffix(int number);
    std::string getOrdinalString(int number);

public:
    explicit PositionHud(const SdlWindow& window);

    void render(int position, int x, int y);
};

#endif  // POSITION_HUD_H
