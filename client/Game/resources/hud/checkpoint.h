#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "../../sdl_wrappers/SdlFont.h"
#include "../../sdl_wrappers/SdlObjTexture.h"
#include "../../sdl_wrappers/SdlWindow.h"

class Checkpoint {
private:
    SdlObjTexture texture;
    SdlObjTexture meta;
    SdlFont text;
    int iteration_init_angle;

public:
    explicit Checkpoint(const SdlWindow& window);

    void render(int x_checkpoint, int y_checkpoint, const Area& src_area_map, int iteration, bool is_meta);

    void renderRemainingHud(int x_checkpoint, int y_checkpoint, int amount, int iteration);
};

#endif  // CHECKPOINT_H
