#include "SdlWindow.h"

#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

#include "SdlException.h"


SdlWindow::SdlWindow(int width, int height): width(width), height(height) {
    int errCode = SDL_Init(SDL_INIT_VIDEO);
    if (errCode) {
        throw SdlException("Initialization error", SDL_GetError());
    }
    errCode = SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &this->window,
                                          &this->renderer);
    if (errCode) {
        throw SdlException("Error creating window", SDL_GetError());
    }

    SDL_DestroyRenderer(renderer);
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    SDL_RenderSetLogicalSize(renderer, width, height);
}


SdlWindow::~SdlWindow() {
    std::cout << "Destroying" << std::endl;
    if (this->renderer) {
        SDL_DestroyRenderer(this->renderer);
        this->renderer = nullptr;
    }

    if (this->window) {
        SDL_DestroyWindow(this->window);
        this->window = nullptr;
    }
}

void SdlWindow::fill(int r, int g, int b, int alpha) {
    SDL_SetRenderDrawColor(this->renderer, r, g, b, alpha);
    SDL_RenderClear(this->renderer);
}

void SdlWindow::fill() { this->fill(0x33, 0x33, 0x33, 0xFF); }

void SdlWindow::render() { SDL_RenderPresent(this->renderer); }


SDL_Renderer* SdlWindow::getRenderer() const { return this->renderer; }
