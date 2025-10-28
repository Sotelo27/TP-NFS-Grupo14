#ifndef __RGB_H__
#define __RGB_H_

class Rgb {
private:
    int r, g, b;

public:
    Rgb(int red, int green, int blue);
    int getR() const;
    int getG() const;
    int getB() const;
};

#endif
