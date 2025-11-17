#ifndef INTERMISSION_H
#define INTERMISSION_H

#include "../sdl_wrappers/SdlObjTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../sdl_wrappers/SdlFont.h"

#include <list>

struct PlayerInfoI {
    int position;
    std::string name;
    int race_time_seconds;
    int total_time_seconds;
};

class Intermission {
private:
    bool active;
    int iterations;
    SdlObjTexture background_texture;
    SdlFont text;

public:
    explicit Intermission(const SdlWindow& window);

    void render();

    void reset();
};

#endif
