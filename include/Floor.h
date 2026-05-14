#ifndef FLOOR_H
#define FLOOR_H

#include <raylib.h>
#include "Constants.h"

class Floor {
public:
    Floor(Vector3 center, float width, float depth, Color color);
    void Draw() const;
    
private:
    Vector3 center;
    float width;
    float depth;
    Color color;
};

#endif