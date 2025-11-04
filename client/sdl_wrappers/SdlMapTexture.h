#ifndef __SDL_MAP_TEXTURE_H__
#define __SDL_MAP_TEXTURE_H__
#include <string>

#include "SdlBaseTexture.h"

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;
class Area;

class SdlMapTexture: public SdlBaseTexture {
protected:
    SDL_Texture* loadTexture(const std::string& filename) final;

public:
    SdlMapTexture(const std::string& filename, const SdlWindow& window);

    SDL_Texture* getTexture() const { return this->texture; }
};

#endif
