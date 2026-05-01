#include "Room.h"
Room::Room() 
    : id(-1)
    , gridX(-1)
    , gridY(-1)
    , worldPosition{0, 0, 0}
    , explored(false)
    , hasKey(false)
    , keyType(KeyType::NONE) {
    for (int i = 0; i < 4; ++i) {
        hasDoor[i] = false;
        doorKey[i] = KeyType::NONE;
        neighbors[i] = nullptr;
    }
}

Room::Room(int x, int y) 
    : id(-1)
    , gridX(x)
    , gridY(y)
    , worldPosition{-x * 10.0f, 0.0f, y * 10.0f}
    , explored(false)
    , hasKey(false)
    , keyType(KeyType::NONE) {
    for (int i = 0; i < 4; ++i) {
        hasDoor[i] = false;
        doorKey[i] = KeyType::NONE;
        neighbors[i] = nullptr;
    }
}

Color GetKeyColor(KeyType type) {
    switch(type) {
        case KeyType::KEY_RED:   return RED;
        case KeyType::KEY_BLUE:  return BLUE;
        case KeyType::KEY_GREEN: return GREEN;
        case KeyType::KEY_GOLD:  return GOLD;
        default:                 return WHITE;
    }
}

const char* GetKeyName(KeyType type) {
    switch(type) {
        case KeyType::KEY_RED:   return "RED";
        case KeyType::KEY_BLUE:  return "BLUE";
        case KeyType::KEY_GREEN: return "GREEN";
        case KeyType::KEY_GOLD:  return "GOLD";
        default:                 return "NONE";
    }
}