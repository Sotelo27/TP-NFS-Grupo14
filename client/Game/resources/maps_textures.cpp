#include "maps_textures.h"

#include <stdexcept>

#include <SDL2/SDL.h>


MapsTextures::MapsTextures(const SdlWindow& window): window(window) {
    // Inicializar los nombres de archivo de los mapas
    mapFilenames[MapID::LibertyCity] = std::string(ASSETS_PATH) + "/cities/LibertyCity.png";
    mapFilenames[MapID::SanAndreas] = std::string(ASSETS_PATH) + "/cities/SanAndreas.png";
    mapFilenames[MapID::ViceCity] = std::string(ASSETS_PATH) + "/cities/ViceCity.png";
}

void MapsTextures::loadMap(MapID mapID) {
    auto it = mapFilenames.find(mapID);
    if (it == mapFilenames.end()) {
        throw std::runtime_error("Invalid MapID provided");
    }

    currentMapTexture = std::make_unique<SdlMapTexture>(it->second, window);
}

void MapsTextures::render(const Area& src, const Area& dest) const {
    if (!currentMapTexture) {
        throw std::runtime_error("No map loaded for rendering");
    }
    currentMapTexture->render(src, dest);
}

int MapsTextures::getCurrentMapWidth() const {
    if (!currentMapTexture) {
        throw std::runtime_error("No map loaded to get width");
    }
    return currentMapTexture->getWidth();
}

int MapsTextures::getCurrentMapHeight() const {
    if (!currentMapTexture) {
        throw std::runtime_error("No map loaded to get height");
    }
    return currentMapTexture->getHeight();
}
