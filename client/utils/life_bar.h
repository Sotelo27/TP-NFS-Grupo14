#ifndef LIFE_BAR_SPRITE_SHEET_H
#define LIFE_BAR_SPRITE_SHEET_H

#include <string>
#include <unordered_map>

#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"

#include "Area.h"

enum class PercentageLifeBar { TEN, TWENTY_FIVE, FIFTY, EIGHTY, FULL };

struct LifeBarData {
    Area area;
    int width_scale_screen;
    int height_scale_screen;
};

class LifeBarSpriteSheet {
private:
    SdlObjTexture texture_life_bar;
    std::unordered_map<PercentageLifeBar, LifeBarData> sprites;

public:
    explicit LifeBarSpriteSheet(const SdlWindow& window);

    void render(int max_life, int current_life, const Area& dest) const;
};

#endif  // LIFE_BAR_SPRITE_SHEET_H
