#ifndef MAPS_TEXTURES_H
#define MAPS_TEXTURES_H

#include <memory>
#include <string>
#include <unordered_map>

#include "../sdl_wrappers/SdlMapTexture.h"
#include "../sdl_wrappers/SdlWindow.h"
#include "../../common/enum/map_enum.h"

#include "Area.h"

struct MapData {
    int width_scale_screen;
    int height_scale_screen;
};

class MapsTextures {
private:
    std::unordered_map<MapID, std::string> mapFilenames;
    const SdlWindow& window;
    std::unique_ptr<SdlMapTexture> currentMapTexture;
    MapData currentMapData;

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
        Retorna los datos del mapa actualmente cargado.

        Si no se ha cargado ningún mapa, lanza una excepción.
     */
    const MapData& getCurrentMapData() const;

    /*
        Renderiza en la ventana la región src del mapa cargado en el área dest.

        Si no se ha cargado ningún mapa, lanza una excepción.
    */
    void render(const Area& src, const Area& dest) const;
};

#endif  // MAPS_TEXTURES_H
