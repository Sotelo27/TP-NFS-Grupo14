#include "life_bar.h"

#include <algorithm>

#define LIFE_BAR_SPRITE_SHEET_FILE std::string(ASSETS_PATH) + "/images/barra_de_vida2.png"

#define LIFE_BAR_WIDTH 100
#define LIFE_BAR_HEIGHT 20

LifeBarSpriteSheet::LifeBarSpriteSheet(const SdlWindow& window):
        texture_life_bar(LIFE_BAR_SPRITE_SHEET_FILE, window, Rgb(0, 0, 255)) {
    sprites.emplace(PercentageLifeBar::FULL,
                    LifeBarData{Area(0, 57, 600, 100), LIFE_BAR_WIDTH, LIFE_BAR_HEIGHT});
    sprites.emplace(PercentageLifeBar::EIGHTY,
                    LifeBarData{Area(0, 157, 600, 100), LIFE_BAR_WIDTH, LIFE_BAR_HEIGHT});
    sprites.emplace(PercentageLifeBar::FIFTY,
                    LifeBarData{Area(0, 257, 600, 100), LIFE_BAR_WIDTH, LIFE_BAR_HEIGHT});
    sprites.emplace(PercentageLifeBar::TWENTY_FIVE,
                    LifeBarData{Area(0, 357, 600, 100), LIFE_BAR_WIDTH, LIFE_BAR_HEIGHT});
    sprites.emplace(PercentageLifeBar::TEN,
                    LifeBarData{Area(0, 457, 600, 100), LIFE_BAR_WIDTH, LIFE_BAR_HEIGHT});
}

void LifeBarSpriteSheet::render(int max_life, int current_life, const Area& dest) const {
    PercentageLifeBar percentage;
    double life_ratio = (max_life > 0) ?
                                static_cast<double>(current_life) / static_cast<double>(max_life) :
                                0.0;

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

    const LifeBarData& life_bar_data = this->sprites.at(percentage);
    this->texture_life_bar.render(life_bar_data.area, dest);
}
