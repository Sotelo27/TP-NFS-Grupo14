#include "SdlObjTexture.h"

#include <string>

#include <SDL2/SDL_image.h>

#include "SdlException.h"
#include "SdlWindow.h"

SdlObjTexture::SdlObjTexture(const std::string& filename, const SdlWindow& window,
                             const Rgb& background_color):
        SdlBaseTexture(window), background_color(background_color) {
    this->texture = loadTexture(filename);
}

SDL_Texture* SdlObjTexture::loadTexture(const std::string& filename) {
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        throw SdlException("Error loading surface", SDL_GetError());
    }
    if (SDL_SetColorKey(surface, SDL_TRUE,
                        SDL_MapRGB(surface->format, background_color.getR(),
                                   background_color.getG(), background_color.getB())) != 0) {
        SDL_FreeSurface(surface);
        throw SdlException("Error setting color key", SDL_GetError());
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!tex) {
        throw SdlException("Error creating texture from surface", SDL_GetError());
    }

    return tex;
}
