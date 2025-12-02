#include "checkpoint.h"

#include "../../constants.h"
#include "../../utils/rgb.h"

#define CHECKPOINT_IMAGE_PATH std::string(ASSETS_PATH) + "/hud/checkpoint.png"
#define META_IMAGE_PATH std::string(ASSETS_PATH) + "/hud/meta.png"

#define SIZE_CHECKPOINT 200

#define SIZE_HUD WINDOW_WIDTH / 43

Checkpoint::Checkpoint(const SdlWindow& window):
        texture(CHECKPOINT_IMAGE_PATH, window, RGB_BACKGROUND),
        meta(META_IMAGE_PATH, window, RGB_BACKGROUND),
        text(FONT_STYLE_AA, (SIZE_HUD * 11) / 12, window),
        iteration_init_angle(0) {
    texture.changeAlpha(200);
}

void Checkpoint::render(int x_checkpoint, int y_checkpoint, const Area& src_area_map,
                        int iteration, bool is_meta) {
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
                              y_checkpoint_screen - SIZE_CHECKPOINT / 2, SIZE_CHECKPOINT,
                              SIZE_CHECKPOINT);

    if (iteration - iteration_init_angle >= 360) {
        iteration_init_angle = iteration;
    }

    float angle = iteration - iteration_init_angle;
    
    if (is_meta) {
        meta.renderEntity(Area(0, 0, meta.getWidth(), meta.getHeight()), dest_area_checkpoint, 0);
    } else {
        texture.renderEntity(Area(0, 0, texture.getWidth(), texture.getHeight()), dest_area_checkpoint,
                         angle);
    }
}

void Checkpoint::renderRemainingHud(int x_checkpoint, int y_checkpoint, int amount, int iteration) {
    if (iteration - iteration_init_angle >= 360) {
        iteration_init_angle = iteration;
    }

    float angle = iteration - iteration_init_angle;
    this->texture.renderEntity(Area(0, 0, texture.getWidth(), texture.getHeight()),
                               Area(x_checkpoint, y_checkpoint, SIZE_HUD,
                                    SIZE_HUD * texture.getHeight() / texture.getWidth()),
                               angle);

    text.renderDirect(x_checkpoint + SIZE_HUD + 5, y_checkpoint + SIZE_HUD / 12,
                      std::to_string(amount), Rgb(255, 255, 255), true);
}
