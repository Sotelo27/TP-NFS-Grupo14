#ifndef __SDL_FONT_TEXTURE_H__
#define __SDL_FONT_TEXTURE_H__
#include <string>

#include <SDL2/SDL_ttf.h>

#include "../utils/Area.h"
#include "../utils/rgb.h"

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;

class SdlFont {
private:
    TTF_Font* font;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
    int height;

    void updateLimits();

public:
    SdlFont(const std::string& filename, size_t font_size, const SdlWindow& window);

    void loadText(const std::string& text, const Rgb& color, bool isBordered = false);
    int render(int x, int y) const;
    void renderDirect(int x, int y, const std::string& text, const Rgb& color, bool shadow = false,
                      const Rgb& shadowColor = Rgb(0, 0, 0));
    int getWidth() const;
    int getHeight() const;

    ~SdlFont();

    SdlFont(const SdlFont&) = delete;
    SdlFont& operator=(const SdlFont&) = delete;
};

#endif
