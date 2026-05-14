#include "Coin.h"
#include "Constants.h"

Coin::Coin(Vector3 pos) : position(pos), collected(false), radius(0.5f) {}

void Coin::Draw() const {
    if (collected) return;
    DrawCylinder(position, radius, radius, 0.2f, 16, YELLOW);
    DrawSphere({position.x, position.y + 0.15f, position.z}, 0.1f, GOLD);
}

BoundingBox Coin::GetBoundingBox() const {
    Vector3 min = {position.x - radius, position.y - 0.1f, position.z - radius};
    Vector3 max = {position.x + radius, position.y + 0.3f, position.z + radius};
    return {min, max};
}