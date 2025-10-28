#ifndef __SDL_OBJ_TEXTURE_H__
#define __SDL_OBJ_TEXTURE_H__
#include <string>

#include "../utils/rgb.h"

#include "SdlBaseTexture.h"

class SDL_Surface;
class SDL_Renderer;
class SdlWindow;
class Area;

class SdlObjTexture: public SdlBaseTexture {
private:
    const Rgb& background_color;

protected:
    SDL_Texture* loadTexture(const std::string& filename) final;

public:
    SdlObjTexture(const std::string& filename, const SdlWindow& window,
                  const Rgb& background_color);
};

#endif
