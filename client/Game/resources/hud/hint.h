#ifndef HINT_H
#define HINT_H

#include "../../sdl_wrappers/SdlObjTexture.h"
#include "../../sdl_wrappers/SdlWindow.h"

class Hint {
private:
    SdlObjTexture texture;
    int phase;

    void adjustAlphaForAnimation(int arrow_index);
public:
    explicit Hint(const SdlWindow& window);

    void render(int x_car, int y_car, int distance_checkpoint, double angle, int iteration,
                int car_width_scale_screen, int car_height_scale_screen);
};

#endif  // HINT_H
