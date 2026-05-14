#ifndef DOOR_H
#define DOOR_H

#include <raylib.h>
#include "Constants.h"
#include "Room.h"

class Door {
public:
    Door(Vector3 position, Direction dir, bool isOpen = false);
    void Draw() const;
    void DrawHighlight() const;
    BoundingBox GetBoundingBox() const;
    Direction GetDirection() const { return direction; }
    
private:
    Vector3 position;
    Direction direction;
    bool isOpen;
    Color doorColor;
    Color frameColor;
    Color handleColor;
    Vector3 size; // width, height, thickness
};

#endif