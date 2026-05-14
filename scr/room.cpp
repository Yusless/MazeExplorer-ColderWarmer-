#include "Room.h"

Room::Room() 
    : id(-1), gridX(-1), gridY(-1), worldPosition{0,0,0}, explored(false) {
    for (int i = 0; i < 4; ++i) {
        hasDoor[i] = false;
        neighbors[i] = nullptr;
    }
}

Room::Room(int x, int y) 
    : id(-1), gridX(x), gridY(y), worldPosition{x * Constants::ROOM_SIZE, 0.0f, y * Constants::ROOM_SIZE}, explored(false) {
    for (int i = 0; i < 4; ++i) {
        hasDoor[i] = false;
        neighbors[i] = nullptr;
    }
}

void Room::SetDoor(Direction dir, bool has) {
    hasDoor[static_cast<int>(dir)] = has;
}

bool Room::HasDoor(Direction dir) const {
    return hasDoor[static_cast<int>(dir)];
}

Room* Room::GetNeighbor(Direction dir) const {
    return neighbors[static_cast<int>(dir)];
}

void Room::SetNeighbor(Direction dir, Room* neighbor) {
    neighbors[static_cast<int>(dir)] = neighbor;
}