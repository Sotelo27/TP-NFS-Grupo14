#ifndef LIFE_HUD_H
#define LIFE_HUD_H

#include <string>
#include <unordered_map>

#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../utils/Area.h"
#include "../utils/add_text.h"

class LifeHud {
private:
    SdlObjTexture texture;
    AddText add_text;

public:
    explicit LifeHud(const SdlWindow& window);

    void render(int max_life, int current_life, int x, int y) const;
};

#endif  // LIFE_HUD_H
