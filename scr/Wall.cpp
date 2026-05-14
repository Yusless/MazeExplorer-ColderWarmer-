#include "Wall.h"

Wall::Wall(Vector3 position, Vector3 size, Color color)
    : position(position), size(size), color(color) {}

void Wall::Draw() const {
    DrawCube(position, size.x, size.y, size.z, color);
}

void Wall::DrawWires(Color color) const {
    DrawCubeWires(position, size.x, size.y, size.z, color);
}