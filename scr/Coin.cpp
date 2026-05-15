#include "Coin.h"
#include "Constants.h"

Coin::Coin(Vector3 pos) : position(pos), collected(false), radius(0.45f) {}

void Coin::Draw() const {
    if (collected) return;
    DrawCylinder(position, radius, radius, 0.1f, 16, YELLOW);
    DrawSphere({position.x, position.y + 0.08f, position.z}, 0.12f, GOLD);
}

BoundingBox Coin::GetBoundingBox() const {
    Vector3 min = {position.x - radius, position.y - 0.05f, position.z - radius};
    Vector3 max = {position.x + radius, position.y + 0.15f, position.z + radius};
    return {min, max};
}