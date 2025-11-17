#include "checkpoint.h"

#include "../../utils/rgb.h"
#include "../../constants.h"

#define CHECKPOINT_IMAGE_PATH std::string(ASSETS_PATH) + "/hud/punto_rojo.png"
#define SIZE_CHECKPOINT 150

Checkpoint::Checkpoint(const SdlWindow& window)
    : texture(CHECKPOINT_IMAGE_PATH, window, Rgb(0,0,0)) {}

void Checkpoint::render(int x_checkpoint, int y_checkpoint, const Area& src_area_map) const {
    Area extend_area_map(src_area_map.getX() - SIZE_CHECKPOINT,
                         src_area_map.getY() - SIZE_CHECKPOINT,
                         src_area_map.getWidth() + SIZE_CHECKPOINT * 2,
                         src_area_map.getHeight() + SIZE_CHECKPOINT * 2);
    if (x_checkpoint < extend_area_map.getX() ||
        x_checkpoint > extend_area_map.getX() + extend_area_map.getWidth() ||
        y_checkpoint < extend_area_map.getY() ||
        y_checkpoint > extend_area_map.getY() + extend_area_map.getHeight()) {
        return;
    }

    int x_checkpoint_screen = (x_checkpoint - src_area_map.getX()) * MAP_TO_VIEWPORT_SCALE_X;
    int y_checkpoint_screen = (y_checkpoint - src_area_map.getY()) * MAP_TO_VIEWPORT_SCALE_Y;
    Area dest_area_checkpoint(x_checkpoint_screen - SIZE_CHECKPOINT / 2,
                              y_checkpoint_screen - SIZE_CHECKPOINT / 2,
                              SIZE_CHECKPOINT, SIZE_CHECKPOINT);
    texture.render(Area(0, 0, texture.getWidth(), texture.getHeight()), dest_area_checkpoint);
}