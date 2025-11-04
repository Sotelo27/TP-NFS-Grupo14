#include "SdlFont.h"

#include <iostream>

#include <SDL2/SDL_image.h>

#include "SdlException.h"
#include "SdlWindow.h"


SdlFont::SdlFont(const std::string& filename, size_t font_size, const SdlWindow& window):
        font(nullptr), renderer(window.getRenderer()) {
    if (TTF_Init() == -1) {
        throw SdlException("Error initializing SDL_ttf", TTF_GetError());
    }

    font = TTF_OpenFont(filename.c_str(), font_size);
    if (!font) {
        throw SdlException("Error loading font", TTF_GetError());
    }
}

void SdlFont::renderText(const std::string& text, const Rgb& color, const Area& dest) const {
    if (text.empty()) {
        return;
    }

    SDL_Color sdlColor = {static_cast<uint8_t>(color.getR()), static_cast<uint8_t>(color.getG()),
                          static_cast<uint8_t>(color.getB()), static_cast<uint8_t>(color.getA())};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), sdlColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    int result = render(texture, dest);
    SDL_DestroyTexture(texture);

    if (result != 0) {
        throw SdlException("Error rendering text texture", SDL_GetError());
    }
}

int SdlFont::render(SDL_Texture* texture, const Area& dest) const {
    if (!texture) {
        throw SdlException("Null texture provided to render", SDL_GetError());
    }

    SDL_Rect sdlDest = {dest.getX(), dest.getY(), dest.getWidth(), dest.getHeight()};

    SDL_QueryTexture(texture, nullptr, nullptr, &sdlDest.w, &sdlDest.h);
    return SDL_RenderCopy(renderer, texture, nullptr, &sdlDest);
}

SdlFont::~SdlFont() {
    if (this->font) {
        TTF_CloseFont(this->font);
        this->font = nullptr;
    }
}
