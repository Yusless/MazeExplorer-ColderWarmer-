#ifndef WALL_H
#define WALL_H

#include <raylib.h>
#include "Constants.h"

class Wall {
public:
    Wall(Vector3 position, Vector3 size, Color color);
    void Draw() const;
    void DrawWires(Color color) const;
    
private:
    Vector3 position;
    Vector3 size;
    Color color;
};

#endif