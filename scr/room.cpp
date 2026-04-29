#include "room.hpp"

// Конструктор Room
Room::Room(int x, int y) : gridX(x), gridY(y) {
    worldPosition = {-x * 10.0f, 0.0f, y * 10.0f};
}

// getKeyColor
Color GetKeyColor(KeyType type) {
    switch(type) {
        case KeyType::KEY_RED:   return RED;
        case KeyType::KEY_BLUE:  return BLUE;
        case KeyType::KEY_GREEN: return GREEN;
        case KeyType::KEY_GOLD:  return GOLD;
        default:                 return WHITE;
    }
}

// GetKeyName
const char* GetKeyName(KeyType type) {
    switch(type) {
        case KeyType::KEY_RED:   return "RED";
        case KeyType::KEY_BLUE:  return "BLUE";
        case KeyType::KEY_GREEN: return "GREEN";
        case KeyType::KEY_GOLD:  return "GOLD";
        default:                 return "NONE";
    }
}