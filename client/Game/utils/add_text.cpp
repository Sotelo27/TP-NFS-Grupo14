#include "add_text.h"

#define FONT_STYLE std::string(ASSETS_PATH) + "/font/PrStart.ttf"

AddText::AddText(size_t font_size, const SdlWindow& window): font(FONT_STYLE, font_size, window) {}

void AddText::renderText(const std::string& text, const Rgb& color, const Area& dest) const {
    font.renderText(text, color, dest);
}
