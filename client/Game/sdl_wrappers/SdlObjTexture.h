#ifndef __SDL_OBJ_TEXTURE_H__
#define __SDL_OBJ_TEXTURE_H__
#include <string>
#include <cstdint>

#include "../utils/Area.h"
#include "../utils/rgb.h"

#include "SdlBaseTexture.h"

class SDL_Surface;
class SDL_Renderer;
class SdlWindow;


class SdlObjTexture: public SdlBaseTexture {
private:
    const Rgb& background_color;

protected:
    SDL_Texture* loadTexture(const std::string& filename) final;

public:
    SdlObjTexture(const std::string& filename, const SdlWindow& window,
                  const Rgb& background_color);

    void renderEntity(const Area& src, const Area& dest, double angle) const;
    void changeAlpha(uint8_t alpha);
};

#endif
