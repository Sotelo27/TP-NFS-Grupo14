#include "SdlBaseTexture.h"

#include <string>

#include <SDL2/SDL_image.h>

#include "../utils/Area.h"

#include "SdlException.h"
#include "SdlWindow.h"

SdlBaseTexture::SdlBaseTexture(const SdlWindow& window):
        renderer(window.getRenderer()), texture(nullptr), width(0), height(0) {}

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

void SdlBaseTexture::updateLimits() {
    if (!this->texture) {
        throw SdlException("Texture not loaded", "Cannot update limits of a null texture");
    }

    SDL_QueryTexture(this->texture, NULL, NULL, &this->width, &this->height);
}

int SdlBaseTexture::getWidth() const { return this->width; }

int SdlBaseTexture::getHeight() const { return this->height; }
