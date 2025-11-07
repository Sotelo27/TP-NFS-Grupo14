#include "SdlDrawFill.h"

#include <SDL2/SDL_image.h>

#include "../utils/Area.h"
#include "../utils/rgb.h"

#include "SdlException.h"
#include "SdlWindow.h"

SdlDrawFill::SdlDrawFill(const SdlWindow& window):
        renderer(window.getRenderer()) {}

void SdlDrawFill::fill(const Area& area, const Rgb& color) const {
    SDL_SetRenderDrawColor(this->renderer, color.getR(), color.getG(), color.getB(), color.getA());

    SDL_Rect sdlArea = {area.getX(), area.getY(), area.getWidth(), area.getHeight()};

    SDL_RenderFillRect(this->renderer, &sdlArea);
}
