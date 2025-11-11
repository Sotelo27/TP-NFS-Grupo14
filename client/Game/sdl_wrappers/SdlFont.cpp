#include "SdlFont.h"

#include <iostream>

#include <SDL2/SDL_image.h>

#include "SdlException.h"
#include "SdlWindow.h"


SdlFont::SdlFont(const std::string& filename, size_t font_size, const SdlWindow& window):
        font(nullptr), renderer(window.getRenderer()), texture(nullptr), width(0), height(0) {
    if (TTF_Init() == -1) {
        throw SdlException("Error initializing SDL_ttf", TTF_GetError());
    }

    font = TTF_OpenFont(filename.c_str(), font_size);
    if (!font) {
        throw SdlException("Error loading font", TTF_GetError());
    }
}

void SdlFont::loadText(const std::string& text, const Rgb& color, bool isBordered) {
    if (this->texture) {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }

    int borderOffset = isBordered ? 2 : 0;
    TTF_SetFontOutline(this->font, borderOffset);

    SDL_Color sdlColor = {static_cast<uint8_t>(color.getR()), static_cast<uint8_t>(color.getG()),
                          static_cast<uint8_t>(color.getB()), static_cast<uint8_t>(color.getA())};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), sdlColor);
    this->texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    this->updateLimits();
}

int SdlFont::render(int x, int y) const {
    if (!texture) {
        throw SdlException("Null texture provided to render", SDL_GetError());
    }

    SDL_Rect sdlDest = {x, y, this->width, this->height};

    return SDL_RenderCopy(renderer, texture, nullptr, &sdlDest);
}

void SdlFont::updateLimits() {
    if (!this->texture) {
        throw SdlException("Texture not loaded", "Cannot update limits of a null texture");
    }

    SDL_QueryTexture(this->texture, NULL, NULL, &this->width, &this->height);
}

int SdlFont::getWidth() const { return this->width; }

int SdlFont::getHeight() const { return this->height; }

SdlFont::~SdlFont() {
    if (this->font) {
        TTF_CloseFont(this->font);
        this->font = nullptr;
    }
    if (this->texture) {
        SDL_DestroyTexture(this->texture);
        this->texture = nullptr;
    }
}

void SdlFont::renderDirect(int x, int y, const std::string& text, const Rgb& color, bool shadow,
                           const Rgb& shadowColor) {
    if (shadow) {
        this->loadText(text, shadowColor, true);
        this->render(x, y);
    }
    this->loadText(text, color);
    this->render(x, y);
}
