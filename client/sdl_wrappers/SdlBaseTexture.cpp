#include "SdlBaseTexture.h"

#include <string>

#include <SDL2/SDL_image.h>

#include "../utils/Area.h"

#include "SdlException.h"
#include "SdlWindow.h"

SdlBaseTexture::SdlBaseTexture(const SdlWindow& window):
        renderer(window.getRenderer()), texture(nullptr) {}

SdlBaseTexture::~SdlBaseTexture() {
    if (this->texture) {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }
}

int SdlBaseTexture::render(const Area& src, const Area& dest) const {
    SDL_Rect sdlSrc = {src.getX(), src.getY(), src.getWidth(), src.getHeight()};
    SDL_Rect sdlDest = {dest.getX(), dest.getY(), dest.getWidth(), dest.getHeight()};

    return SDL_RenderCopy(this->renderer, this->texture, &sdlSrc, &sdlDest);
}
