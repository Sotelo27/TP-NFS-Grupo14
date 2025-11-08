#ifndef __RGB_H__
#define __RGB_H_

#pragma once

class Rgb {
private:
    int r, g, b, a;

public:
    Rgb(int red, int green, int blue);
    Rgb(int red, int green, int blue, int alpha);

    int getR() const;
    int getG() const;
    int getB() const;
    int getA() const;
};

#endif
