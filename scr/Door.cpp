#include "Door.h"

Door::Door(Vector3 position, Direction dir, bool isOpen)
    : position(position), direction(dir), isOpen(isOpen),
      doorColor{139, 69, 19, 255},
      frameColor{101, 67, 33, 255},
      handleColor(GOLD) {
    // N/S: проём вдоль X — ширина по X, толщина по Z.
    // E/W: проём вдоль Z — ширина по Z, толщина по X.
    constexpr float thickness = 0.2f;
    switch (dir) {
        case Direction::EAST:
        case Direction::WEST:
            size = {thickness, Constants::DOOR_HEIGHT, Constants::DOOR_WIDTH};
            break;
        default:
            size = {Constants::DOOR_WIDTH, Constants::DOOR_HEIGHT, thickness};
            break;
    }
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
    // Pad only in the horizontal plane; keep height = door height so the outline does not stick into the lintel.
    constexpr float padAlongOpening = 0.1f;
    constexpr float padThickness = 0.06f;
    switch (direction) {
        case Direction::EAST:
        case Direction::WEST:
            DrawCubeWires(position, size.x + padThickness * 2.0f, size.y, size.z + padAlongOpening * 2.0f, YELLOW);
            break;
        default:
            DrawCubeWires(position, size.x + padAlongOpening * 2.0f, size.y, size.z + padThickness * 2.0f, YELLOW);
            break;
    }
}

BoundingBox Door::GetBoundingBox() const {
    Vector3 min = {position.x - size.x/2, position.y - size.y/2, position.z - size.z/2};
    Vector3 max = {position.x + size.x/2, position.y + size.y/2, position.z + size.z/2};
    return {min, max};
}

BoundingBox Door::GetPickBoundingBox() const {
    constexpr float padThin = 0.35f;
    float hx = size.x * 0.5f;
    float hy = size.y * 0.5f;
    float hz = size.z * 0.5f;
    switch (direction) {
        case Direction::EAST:
        case Direction::WEST:
            hx += padThin;
            break;
        default:
            hz += padThin;
            break;
    }
    return {{position.x - hx, position.y - hy, position.z - hz},
            {position.x + hx, position.y + hy, position.z + hz}};
}

Direction PickClosestDoorAlongRay(const Room* room, Ray ray) {
    if (!room) return Direction::NONE;

    Vector3 pos = room->GetWorldPosition();
    float half = Constants::ROOM_SIZE / 2.0f;
    constexpr float kMinDist = 0.001f;
    float bestDist = 1.0e30f;
    Direction best = Direction::NONE;

    auto tryDoor = [&](Direction dir, Vector3 doorCenter) {
        if (!room->HasDoor(dir)) return;
        Door door(doorCenter, dir);
        RayCollision c = GetRayCollisionBox(ray, door.GetPickBoundingBox());
        if (c.hit && c.distance >= kMinDist && c.distance < bestDist) {
            bestDist = c.distance;
            best = dir;
        }
    };

    tryDoor(Direction::NORTH, {pos.x, Constants::DOOR_HEIGHT / 2.0f, pos.z + half});
    tryDoor(Direction::SOUTH, {pos.x, Constants::DOOR_HEIGHT / 2.0f, pos.z - half});
    tryDoor(Direction::EAST, {pos.x + half, Constants::DOOR_HEIGHT / 2.0f, pos.z});
    tryDoor(Direction::WEST, {pos.x - half, Constants::DOOR_HEIGHT / 2.0f, pos.z});

    return best;
}