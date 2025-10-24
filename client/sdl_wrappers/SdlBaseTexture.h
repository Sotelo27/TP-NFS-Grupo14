#ifndef __SDL_BASE_TEXTURE_H__
#define __SDL_BASE_TEXTURE_H__
#include <string>

class SDL_Texture;
class SDL_Renderer;
class SdlWindow;
class Area;

class SdlBaseTexture {
protected:
    virtual SDL_Texture* loadTexture(const std::string &filename) = 0;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

public:
    /**
     * Crea un SDL_Texture, lanza una excepción si el filename es inválido
     **/
    SdlBaseTexture(const SdlWindow& window);
    /**
     * Libera la memoria reservada por la textura
     **/
    virtual ~SdlBaseTexture();
    /**
     * Renderiza la textura cargada
     **/
    int render(const Area& src, const Area& dest) const;
};

#endif
