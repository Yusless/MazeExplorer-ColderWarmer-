#ifndef ROOM_HPP
#define ROOM_HPP

#include <raylib.h>

enum class Direction { 
    NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3, NONE = -1
};

struct Room {
    int id;
    int gridX, gridY;
    Vector3 worldPosition;
    bool explored = false;
    
    bool hasDoor[4] = {false, false, false, false};
    Room* neighbors[4] = {nullptr, nullptr, nullptr, nullptr};
     
    Room();
    Room(int x, int y);
};

#endif