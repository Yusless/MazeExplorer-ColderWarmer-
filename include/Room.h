#ifndef ROOM_HPP
#define ROOM_HPP

#include <raylib.h>

enum class Direction { 
    NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3, NONE = -1
};

enum class KeyType {
    NONE = -1, KEY_RED = 0, KEY_BLUE = 1, KEY_GREEN = 2, KEY_GOLD = 3
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
    
    Room(); 
    Room(int x, int y);
};

// Только объявления функций
Color GetKeyColor(KeyType type);
const char* GetKeyName(KeyType type);

#endif