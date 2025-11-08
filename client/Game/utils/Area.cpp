#include "Area.h"

Area::Area(int x, int y, int width, int height): x(x), y(y), width(width), height(height) {}

int Area::getX() const { return this->x; }
int Area::getY() const { return this->y; }
int Area::getWidth() const { return this->width; }
int Area::getHeight() const { return this->height; }

void Area::update(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}
