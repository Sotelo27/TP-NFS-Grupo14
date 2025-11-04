#include "rgb.h"

Rgb::Rgb(int r, int g, int b): r(r), g(g), b(b), a(255) {}

Rgb::Rgb(int r, int g, int b, int a): r(r), g(g), b(b), a(a) {}

int Rgb::getR() const { return r; }

int Rgb::getG() const { return g; }

int Rgb::getB() const { return b; }

int Rgb::getA() const { return a; }
