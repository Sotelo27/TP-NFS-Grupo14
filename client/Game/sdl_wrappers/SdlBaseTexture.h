#ifndef __SDL_BASE_TEXTURE_H__
#define __SDL_BASE_TEXTURE_H__
#include <string>

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;
class Area;

class SdlBaseTexture {
protected:
    virtual SDL_Texture* loadTexture(const std::string& filename) = 0;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int width;
    int height;

    void updateLimits();

public:
    explicit SdlBaseTexture(const SdlWindow& window);
    virtual ~SdlBaseTexture();
    int render(const Area& src, const Area& dest) const;
    int getWidth() const;
    int getHeight() const;
};

#endif
