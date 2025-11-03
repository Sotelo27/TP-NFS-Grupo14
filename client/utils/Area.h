#ifndef __AREA_H__
#define __AREA_H__

#pragma once

class Area {
private:
    int x, y;
    int width, height;

public:
    Area() {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
    }
    Area(int x, int y, int width, int height);
    int getX() const;
    int getY() const;
    int getWidth() const;
    int getHeight() const;
    void update(int x, int y, int width, int height);
};

#endif
