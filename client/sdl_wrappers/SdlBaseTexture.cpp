#include "SdlBaseTexture.h"
#include <SDL2/SDL_image.h>
#include <string>
#include "SdlWindow.h"
#include "SdlException.h"

SdlBaseTexture::SdlBaseTexture(const SdlWindow& window)
    : renderer(window.getRenderer()) {}

SdlBaseTexture::~SdlBaseTexture() {
    SDL_DestroyTexture(this->texture);
}

int SdlBaseTexture::render(const Area& src, const Area& dest) const {
    SDL_Rect sdlSrc = {
            src.getX(), src.getY(),
            src.getWidth(), src.getHeight()
    };
    SDL_Rect sdlDest = {
            dest.getX(), dest.getY(),
            dest.getWidth(), dest.getHeight()
    };

    return SDL_RenderCopy(this->renderer, this->texture, &sdlSrc, &sdlDest);
}