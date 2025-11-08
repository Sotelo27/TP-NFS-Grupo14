#ifndef __SDL_DRAW_FILL_H__
#define __SDL_DRAW_FILL_H__
#include <string>

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;
class Area;
class Rgb;

class SdlDrawFill {
private:
    SDL_Renderer* renderer;

public:
    explicit SdlDrawFill(const SdlWindow& window);

    void fill(const Area& area, const Rgb& color) const;
};

#endif
