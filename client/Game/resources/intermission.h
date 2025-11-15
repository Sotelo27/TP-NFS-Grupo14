#ifndef INTERMISSION_H
#define INTERMISSION_H

#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"

class Intermission {
private:
    bool active;
    int iterations;
    SdlObjTexture background_texture;

public:
    explicit Intermission(const SdlWindow& window);

    void render();

    void reset();
};

#endif
