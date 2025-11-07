#include "maps_textures.h"

#include <stdexcept>

#include <SDL2/SDL.h>


MapsTextures::MapsTextures(const SdlWindow& window): window(window) {
    // Inicializar los nombres de archivo de los mapas
    mapFilenames[MapID::LibertyCity] = std::string(ASSETS_PATH) + "/cities/Liberty City.png";
    mapFilenames[MapID::SanAndreas] = std::string(ASSETS_PATH) + "/cities/San Andreas.png";
    mapFilenames[MapID::ViceCity] = std::string(ASSETS_PATH) + "/cities/Vice City.png";
}

void MapsTextures::loadMap(MapID mapID) {
    auto it = mapFilenames.find(mapID);
    if (it == mapFilenames.end()) {
        throw std::runtime_error("Invalid MapID provided");
    }

    currentMapTexture = std::make_unique<SdlMapTexture>(it->second, window);

    SDL_QueryTexture(currentMapTexture->getTexture(), NULL, NULL,
                     &currentMapData.width_scale_screen, &currentMapData.height_scale_screen);
}

const MapData& MapsTextures::getCurrentMapData() const {
    if (!currentMapTexture) {
        throw std::runtime_error("No map loaded for getting data");
    }
    return currentMapData;
}

void MapsTextures::render(const Area& src, const Area& dest) const {
    if (!currentMapTexture) {
        throw std::runtime_error("No map loaded for rendering");
    }
    currentMapTexture->render(src, dest);
}
