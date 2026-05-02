#include "Room.h"
Room::Room() 
    : id(-1)
    , gridX(-1)
    , gridY(-1)
    , worldPosition{0, 0, 0}
    , explored(false) {
    for (int i = 0; i < 4; ++i) {
        hasDoor[i] = false;
        neighbors[i] = nullptr;
    }
}

Room::Room(int x, int y) 
    : id(-1)
    , gridX(x)
    , gridY(y)
    , worldPosition{-x * 10.0f, 0.0f, y * 10.0f}
    , explored(false) {
    for (int i = 0; i < 4; ++i) {
        hasDoor[i] = false;
        neighbors[i] = nullptr;
    }
}