#ifndef MAPS_TEXTURES_H
#define MAPS_TEXTURES_H

#include <memory>
#include <string>
#include <unordered_map>

#include "../../../common/enum/map_enum.h"
#include "../sdl_wrappers/SdlMapTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../utils/Area.h"

class MapsTextures {
private:
    std::unordered_map<MapID, std::string> mapFilenames;
    const SdlWindow& window;
    std::unique_ptr<SdlMapTexture> currentMapTexture;

public:
    /*
        Crea el gestor de mapas asociado a la ventana dada.
    */
    explicit MapsTextures(const SdlWindow& window);

    /*
        Carga el mapa correspondiente a mapID, liberando el mapa previamente cargado si
        hubiera alguno.
     */
    void loadMap(MapID mapID);

    /*
        Renderiza en la ventana la región src del mapa cargado en el área dest.

        Si no se ha cargado ningún mapa, lanza una excepción.
    */
    void render(const Area& src, const Area& dest) const;

    int getCurrentMapWidth() const;
    int getCurrentMapHeight() const;
};

#endif  // MAPS_TEXTURES_H
