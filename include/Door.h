
#include <raylib.h>
#include "Constants.h"
#include "Room.h"

class Door {
public:
    Door(Vector3 position, Direction dir);
    void Draw() const;
    void DrawHighlight() const;
    BoundingBox GetBoundingBox() const;
    BoundingBox GetPickBoundingBox() const;
    Direction GetDirection() const { return direction; }
    
private:
    Vector3 position;
    Direction direction;
    bool isOpen;
    Color doorColor;
    Color frameColor;
    Color handleColor;
    Vector3 size;
};

Direction PickClosestDoorAlongRay(const Room* room, Ray ray);