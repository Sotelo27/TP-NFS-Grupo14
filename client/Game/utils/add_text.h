#ifndef ADD_TEXT_H
#define ADD_TEXT_H

#include <string>

#include "../sdl_wrappers/SdlFont.h"
#include "../sdl_wrappers/SdlWindow.h"

#include "Area.h"

class AddText {
private:
    SdlFont font;

public:
    explicit AddText(size_t font_size, const SdlWindow& window);

    void renderText(const std::string& text, const Rgb& color, const Area& dest) const;
};

#endif
