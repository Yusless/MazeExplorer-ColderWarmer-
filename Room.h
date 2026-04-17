// Room.h
#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>

// ВАЖНО: Направления должны соответствовать осям координат
// В raylib: X - вправо, Z - вперёд (север), -Z - назад (юг)
enum class Direction { 
    NORTH = 0, 
    SOUTH = 1, 
    EAST = 2, 
    WEST = 3,
    NONE = -1
};

struct Room {
    int id;
    int gridX, gridY;
    Vector3 worldPosition;
    bool explored = false;
    
    bool hasDoor[4] = {false, false, false, false};
    Room* neighbors[4] = {nullptr, nullptr, nullptr, nullptr};
    
    Room(int x, int y) : gridX(x), gridY(y) {
        // X - вправо, Z - вперёд (как в стандартном 3D)
        worldPosition = {-x * 10.0f, 0.0f, y * 10.0f};
}
};

#endif