#include "SdlMapTexture.h"

#include <SDL2/SDL_image.h>

#include "SdlException.h"


SdlMapTexture::SdlMapTexture(const std::string& filename, const SdlWindow& window):
        SdlBaseTexture(window) {
    this->texture = loadTexture(filename);
}

SDL_Texture* SdlMapTexture::loadTexture(const std::string& filename) {
    SDL_Texture* texture = IMG_LoadTexture(this->renderer, filename.c_str());
    if (!texture) {
        throw SdlException("Error loading texture", SDL_GetError());
    }
    return texture;
}
