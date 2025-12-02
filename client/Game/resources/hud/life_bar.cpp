#include "life_bar.h"

#include <algorithm>

#include "../../constants.h"

#define LIFE_BAR_SPRITE_SHEET_FILE std::string(ASSETS_PATH) + "/hud/barra_de_vida.png"

LifeBarSpriteSheet::LifeBarSpriteSheet(const SdlWindow& window):
        texture_life_bar(LIFE_BAR_SPRITE_SHEET_FILE, window, RGB_BACKGROUND) {
    sprites.emplace(PercentageLifeBar::FULL, Area(123, 94, 767, 165));
    sprites.emplace(PercentageLifeBar::EIGHTY, Area(123, 259, 767, 165));
    sprites.emplace(PercentageLifeBar::FIFTY, Area(123, 424, 767, 165));
    sprites.emplace(PercentageLifeBar::TWENTY_FIVE, Area(123, 589, 767, 160));
    sprites.emplace(PercentageLifeBar::TEN, Area(123, 749, 767, 163));
}

void LifeBarSpriteSheet::render(int max_life, int current_life, const Area& dest) const {
    if (max_life <= 0 || current_life <= 0) {
        return;
    }

    PercentageLifeBar percentage;
    double life_ratio = static_cast<double>(current_life) / static_cast<double>(max_life);

    life_ratio = std::clamp(life_ratio, 0.0, 1.0);

    if (life_ratio >= 0.9) {
        percentage = PercentageLifeBar::FULL;
    } else if (life_ratio >= 0.8) {
        percentage = PercentageLifeBar::EIGHTY;
    } else if (life_ratio >= 0.5) {
        percentage = PercentageLifeBar::FIFTY;
    } else if (life_ratio >= 0.25) {
        percentage = PercentageLifeBar::TWENTY_FIVE;
    } else {
        percentage = PercentageLifeBar::TEN;
    }

    const Area& life_bar_src_area = this->sprites.at(percentage);
    this->texture_life_bar.render(life_bar_src_area, dest);
}
