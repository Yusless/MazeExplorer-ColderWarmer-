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
    /// Slightly thicker along the door slab axis — stable ray picks for N/S (thin in Z).
    BoundingBox GetPickBoundingBox() const;
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

/// Chooses the door actually in front of the camera (smallest positive ray distance).
Direction PickClosestDoorAlongRay(const Room* room, Ray ray);

#endif