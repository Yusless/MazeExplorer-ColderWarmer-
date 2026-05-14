#include "Floor.h"

Floor::Floor(Vector3 center, float width, float depth, Color color)
    : center(center), width(width), depth(depth), color(color) {}

void Floor::Draw() const {
    DrawPlane(center, {width, depth}, color);
}