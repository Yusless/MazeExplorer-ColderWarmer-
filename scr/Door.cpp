#include "Door.h"

Door::Door(Vector3 position, Direction dir, bool isOpen)
    : position(position), direction(dir), isOpen(isOpen),
      doorColor{139, 69, 19, 255},
      frameColor{101, 67, 33, 255},
      handleColor(GOLD) {
    size = {Constants::DOOR_WIDTH, Constants::DOOR_HEIGHT, 0.2f};
}

void Door::Draw() const {
    Color col = isOpen ? DARKGRAY : doorColor;
    DrawCube(position, size.x, size.y, size.z, col);
    DrawCubeWires(position, size.x + 0.01f, size.y + 0.01f, size.z + 0.01f, frameColor);
    
    Vector3 handlePos = position;
    float offset = Constants::DOOR_WIDTH / 3.0f;
    switch(direction) {
        case Direction::NORTH: handlePos.x -= offset; break;
        case Direction::SOUTH: handlePos.x += offset; break;
        case Direction::EAST:  handlePos.z += offset; break;
        case Direction::WEST:  handlePos.z -= offset; break;
        default: break;
    }
    DrawSphere(handlePos, 0.12f, handleColor);
}

void Door::DrawHighlight() const {
    DrawCubeWires(position, size.x + 0.15f, size.y + 0.15f, size.z + 0.02f, YELLOW);
}

BoundingBox Door::GetBoundingBox() const {
    Vector3 min = {position.x - size.x/2, position.y - size.y/2, position.z - size.z/2};
    Vector3 max = {position.x + size.x/2, position.y + size.y/2, position.z + size.z/2};
    return {min, max};
}