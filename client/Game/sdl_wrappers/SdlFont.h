#ifndef __SDL_FONT_TEXTURE_H__
#define __SDL_FONT_TEXTURE_H__
#include <string>

#include <SDL2/SDL_ttf.h>

#include "../utils/Area.h"
#include "../utils/rgb.h"

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;
class Area;

class SdlFont {
private:
    TTF_Font* font;
    SDL_Renderer* renderer;

    int render(SDL_Texture* texture, const Area& dest) const;

public:
    SdlFont(const std::string& filename, size_t font_size, const SdlWindow& window);

    void renderText(const std::string& text, const Rgb& color, const Area& dest) const;

    ~SdlFont();

    SdlFont(const SdlFont&) = delete;
    SdlFont& operator=(const SdlFont&) = delete;
};

#endif
