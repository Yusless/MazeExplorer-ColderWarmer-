// Room.h
#ifndef ROOM_H
#define ROOM_H

#include <raylib.h>

enum class Direction { 
    NORTH = 0, 
    SOUTH = 1, 
    EAST = 2, 
    WEST = 3,
    NONE = -1
};

enum class KeyType {
    NONE = -1,
    KEY_RED = 0,
    KEY_BLUE = 1,
    KEY_GREEN = 2,
    KEY_GOLD = 3
};

struct Room {
    int id;
    int gridX, gridY;
    Vector3 worldPosition;
    bool explored = false;
    
    bool hasDoor[4] = {false, false, false, false};
    KeyType doorKey[4] = {KeyType::NONE, KeyType::NONE, KeyType::NONE, KeyType::NONE};
    Room* neighbors[4] = {nullptr, nullptr, nullptr, nullptr};
    
    bool hasKey = false;
    KeyType keyType = KeyType::NONE;
    
    Room(int x, int y) : gridX(x), gridY(y) {
        worldPosition = {-x * 10.0f, 0.0f, y * 10.0f};
    }
};

inline Color GetKeyColor(KeyType type) {
    switch(type) {
        case KeyType::KEY_RED:   return RED;
        case KeyType::KEY_BLUE:  return BLUE;
        case KeyType::KEY_GREEN: return GREEN;
        case KeyType::KEY_GOLD:  return GOLD;
        default:                 return WHITE;
    }
}

inline const char* GetKeyName(KeyType type) {
    switch(type) {
        case KeyType::KEY_RED:   return "RED";
        case KeyType::KEY_BLUE:  return "BLUE";
        case KeyType::KEY_GREEN: return "GREEN";
        case KeyType::KEY_GOLD:  return "GOLD";
        default:                 return "NONE";
    }
}

#endif